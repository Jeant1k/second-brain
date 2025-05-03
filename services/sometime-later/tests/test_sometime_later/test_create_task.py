import pytest
import json

from testsuite.databases import pgsql
from .plugins.utils import select_user_tasks


@pytest.mark.parametrize(
    'testcase',
    [
        pytest.param('empty_user_id'),
        pytest.param('empty_name'),
        pytest.param('empty_description'),
        pytest.param('happy_path')
    ],
)
async def test_create_task(
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
        '/sometime-later/v1/task',
        json=initial_data['request_body'],
    )

    print(json.dumps(response.json(), indent=2, ensure_ascii=False))

    # assert
    assert response.status == expected_data['status_code']
    assert json.loads(response.text) == expected_data['response']

    assert select_user_tasks(pgsql, initial_data['request_body'].get(
        'user_id', 0)) == expected_data['database_data']
