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
            marks=[pytest.mark.pgsql('sometime_later', files=[
                                     'database_moved_to_current_actions_task.sql'])]
        ),
        pytest.param(
            'task_is_not_in_pending_status',
            marks=[pytest.mark.pgsql('sometime_later', files=[
                                     'database_not_pending_task.sql'])]
        ),
        pytest.param(
            '400_from_current_actions',
            marks=[pytest.mark.pgsql('sometime_later', files=[
                                     'database_pending_task.sql'])]
        ),
        pytest.param(
            '500_from_current_actions',
            marks=[pytest.mark.pgsql('sometime_later', files=[
                                     'database_pending_task.sql'])]
        ),
        pytest.param(
            'happy_path',
            marks=[pytest.mark.pgsql('sometime_later', files=[
                                     'database_pending_task.sql'])]
        ),
    ],
)
async def test_current_actions_task(
    service_client,
    mock_current_actions,
    load_json,
    pgsql,
    testcase,
):
    # arrange
    test_data = load_json(f'testcases/{testcase}.json')
    initial_data = test_data['initial_data']
    expected_data = test_data['expected_result']

    mock_current_actions.set_response(**initial_data['mock_current_actions'])
    mock_current_actions.set_expected_request(
        **expected_data['mock_current_actions'])

    # act
    response = await service_client.post(
        '/sometime-later/v1/task/current/actions',
        json=initial_data['request_body'],
    )

    print(json.dumps(response.json(), indent=2, ensure_ascii=False))

    # assert
    assert response.status == expected_data['status_code']
    assert json.loads(response.text) == expected_data['response']

    assert select_task(pgsql, TASK_ID) == expected_data['database_data']
