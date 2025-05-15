import pytest
import json

from testsuite.databases import pgsql
from .plugins.utils import select_task
from .plugins.constants import TASK_ID


@pytest.mark.parametrize(
    'testcase',
    [
        pytest.param('empty_task_id'),
        pytest.param('task_id_wrong_format'),
        pytest.param('task_not_found'),
        pytest.param(
            'task_already_moved',
            marks=[pytest.mark.pgsql('current_actions', files=[
                                     'database_moved_to_sometime_later_task.sql'])]
        ),
        pytest.param(
            'task_in_inactive_status',
            marks=[pytest.mark.pgsql('current_actions', files=[
                                     'database_inactive_task.sql'])]
        ),
        pytest.param(
            '400_from_sometime_later',
            marks=[pytest.mark.pgsql('current_actions', files=[
                                     'database_active_task.sql'])]
        ),
        pytest.param(
            '500_from_sometime_later',
            marks=[pytest.mark.pgsql('current_actions', files=[
                                     'database_active_task.sql'])]
        ),
        pytest.param(
            'happy_path',
            marks=[pytest.mark.pgsql('current_actions', files=[
                                     'database_active_task.sql'])]
        ),
    ],
)
async def test_sometime_later_task(
    service_client,
    mock_sometime_later,
    load_json,
    pgsql,
    testcase,
):
    # arrange
    test_data = load_json(f'testcases/{testcase}.json')
    initial_data = test_data['initial_data']
    expected_data = test_data['expected_result']

    mock_sometime_later.set_response(**initial_data['mock_sometime_later'])
    mock_sometime_later.set_expected_request(
        **expected_data['mock_sometime_later'])

    # act
    response = await service_client.post(
        '/current-actions/v1/task/sometime/later',
        json=initial_data['request_body'],
    )

    print(json.dumps(response.json(), indent=2, ensure_ascii=False))

    # assert
    assert response.status == expected_data['status_code']
    assert json.loads(response.text) == expected_data['response']

    assert select_task(pgsql, TASK_ID) == expected_data['database_data']
