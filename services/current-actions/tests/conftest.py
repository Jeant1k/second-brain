import pathlib

import pytest
import typing

from testsuite.databases.pgsql import discover


pytest_plugins = ['pytest_userver.plugins.postgresql']


@pytest.fixture(scope='session')
def service_source_dir():
    """Path to root directory service."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Path for find files with data"""
    return [
        service_source_dir / 'tests/test_current_actions/static/test_create_task/testcases',
        service_source_dir / 'tests/test_current_actions/static/test_complete_task/testcases',
        service_source_dir / 'tests/test_current_actions/static/test_reactivate_task/testcases',
    ]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'current-actions',  # service name that goes to the DB connection
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))


class SometimeLaterMocker:
    """Вспомогательный класс для управления моком sometime-later."""

    def __init__(self, mockserver):
        self._mockserver = mockserver
        self._mock_handler = None
        self._status: int = 200
        self._response_json: typing.Dict[str, typing.Any] = {}
        self._expected_request_json: typing.Optional[typing.Dict[str,
                                                                 typing.Any]] = None
        self._last_request_json: typing.Optional[typing.Dict[str,
                                                             typing.Any]] = None
        self._setup_mock()

    def _setup_mock(self):
        """Настраивает обработчик в mockserver."""

        @self._mockserver.json_handler('/internal/sometime-later/v1/task/move')
        def _mock_move_task(request):
            self._last_request_json = request.json

            if self._expected_request_json is not None:
                assert request.json == self._expected_request_json, (
                    f"Тело запроса к sometime-later не совпало.\n"
                    f"Ожидалось: {self._expected_request_json}\n"
                    f"Получено: {request.json}"
                )

            return self._mockserver.make_response(
                status=self._status, json=self._response_json,
            )

        self._mock_handler = _mock_move_task

    def set_response(self, status: int = 200, response_json: typing.Dict[str, typing.Any] = None):
        """
        Задает ожидаемый статус и тело JSON ответа от мока.

        Args:
            status: HTTP статус код (например, 200, 400, 404, 500).
            response_json: Словарь, который будет возвращен как JSON тело.
                           Если None, будет возвращено пустое тело {}.
                           Для 200 OK без тела передавайте {}.
                           Для ошибок используйте формат {'code': '...', 'message': '...'}.
        """
        self._status = status
        self._response_json = response_json if response_json is not None else {}

    def set_expected_request(self, expected_json: typing.Dict[str, typing.Any] = {}):
        """
        Задает ожидаемое тело JSON запроса к моку.
        Если запрос не совпадет, тест упадет с AssertionError.
        """
        self._expected_request_json = expected_json

    @property
    def times_called(self) -> int:
        """Возвращает количество вызовов мока."""
        if not self._mock_handler:
            return 0
        return self._mock_handler.times_called

    @property
    def has_calls(self) -> bool:
        """Возвращает True, если мок был вызван хотя бы раз."""
        return self.times_called > 0

    @property
    def last_request_json(self) -> typing.Optional[typing.Dict[str, typing.Any]]:
        """Возвращает JSON последнего запроса, полученного моком."""
        return self._last_request_json


@pytest.fixture(name='mock_sometime_later')
def _mock_sometime_later(mockserver) -> SometimeLaterMocker:
    """
    Фикстура для мокирования ручки /internal/sometime-later/v1/task/move.

    Возвращает объект SometimeLaterMocker, с помощью которого можно
    настроить ожидаемый ответ (статус и тело) и проверить вызовы.
    """
    return SometimeLaterMocker(mockserver)
