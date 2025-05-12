import pytest
import json

from testsuite.databases import pgsql
from .plugins.utils import select_user_full_tasks
from .plugins.constants import NOW


@pytest.mark.parametrize(
    'testcase',
    [
        pytest.param('empty_id'),
        pytest.param('empty_user_id'),
        pytest.param('empty_name'),
        pytest.param('empty_description'),
        pytest.param('empty_status'),
        pytest.param('wrong_id_format'),
        pytest.param(
            'happy_path_insert_same',
            marks=[pytest.mark.pgsql(
                'waiting', files=['database.sql'])],
        ),
        pytest.param(
            'happy_path_minimal',
            marks=[pytest.mark.pgsql(
                'waiting', files=['database.sql'])],
        ),
        pytest.param('happy_path_insert'),
        pytest.param(
            'happy_path_update',
            marks=[pytest.mark.pgsql(
                'waiting', files=['database.sql'])]
        ),
        pytest.param(
            'happy_path_update_from_other_user',
            marks=[pytest.mark.pgsql(
                'waiting', files=['database.sql'])]
        ),
    ],
)
@pytest.mark.now(NOW)
async def test_move_task(
    service_client,
    load_json,
    pgsql,
    testcase,
):
    # arrange
    test_data = load_json(f'testcases/{testcase}.json')
    initial_data = test_data['initial_data']
    expected_data = test_data['expected_result']

    # act
    response = await service_client.post(
        '/internal/waiting/v1/task/move',
        json=initial_data['request_body'],
    )

    print(json.dumps(response.json(), indent=2, ensure_ascii=False))
    print(json.dumps(select_user_full_tasks(pgsql, initial_data['request_body']['task'].get(
        'user_id', 0)), indent=2, ensure_ascii=False))

    # assert
    assert response.status == expected_data['status_code']
    assert json.loads(response.text) == expected_data['response']

    assert select_user_full_tasks(pgsql, initial_data['request_body']['task'].get(
        'user_id', 0)) == expected_data['database_data']
