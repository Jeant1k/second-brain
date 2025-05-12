import pytest
import json

from testsuite.databases import pgsql
from .plugins.utils import select_task
from .plugins.constants import TASK_ID


@pytest.mark.parametrize(
    'testcase',
    [
        pytest.param('empty_task_id'),
        pytest.param('task_not_found'),
        pytest.param('task_id_wrong_format'),
        pytest.param('happy_path'),
    ],
)
@pytest.mark.pgsql('waiting', files=['database.sql'])
async def test_complete_task(
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
        '/waiting/v1/task/complete',
        json=initial_data['request_body'],
    )

    print(json.dumps(response.json(), indent=2, ensure_ascii=False))

    # assert
    assert response.status == expected_data['status_code']
    assert json.loads(response.text) == expected_data['response']

    assert select_task(pgsql, TASK_ID) == expected_data['database_data']
