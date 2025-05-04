import pytest
import typing


class CurrentActionsMocker:
    """Вспомогательный класс для управления моком current-actions."""

    def __init__(self, mockserver):
        self._mockserver = mockserver
        self._mock_handler = None
        self._status: int = 200
        self._response_json: typing.Dict[str, typing.Any] = {}
        self._expected_request_json: typing.Optional[typing.Dict[str, typing.Any]] = None
        self._last_request_json: typing.Optional[typing.Dict[str, typing.Any]] = None
        self._setup_mock()

    def _setup_mock(self):
        """Настраивает обработчик в mockserver."""

        @self._mockserver.json_handler('/internal/current-actions/v1/task/move')
        def _mock_move_task(request):
            self._last_request_json = request.json

            if self._expected_request_json is not None:
                assert request.json == self._expected_request_json, (
                    f"Тело запроса к current-actions не совпало.\n"
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

    def set_expected_request(self, expected_json: typing.Dict[str, typing.Any]):
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


@pytest.fixture(name='mock_current_actions')
def _mock_current_actions(mockserver) -> CurrentActionsMocker:
    """
    Фикстура для мокирования ручки /internal/current-actions/v1/task/move.

    Возвращает объект CurrentActionsMocker, с помощью которого можно
    настроить ожидаемый ответ (статус и тело) и проверить вызовы.
    """
    return CurrentActionsMocker(mockserver)
