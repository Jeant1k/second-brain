import pytest
import json

from testsuite.databases import pgsql
from .plugins.constants import NOW


@pytest.mark.parametrize(
    'testcase',
    [
        pytest.param('empty_user_id'),
        pytest.param('user_id_not_found'),
        pytest.param('happy_path_minimal'),
        pytest.param('happy_path_by_status'),
        pytest.param('happy_path_by_cursor'),
        pytest.param('happy_path_by_status_and_cursor'),
        pytest.param('happy_path_with_response_cursor'),
    ],
)
@pytest.mark.pgsql('notes', files=['database.sql'])
@pytest.mark.now(NOW)
async def test_list_notes(
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
        '/notes/v1/note/list',
        json=initial_data['request_body'],
    )

    print(json.dumps(response.json(), indent=2, ensure_ascii=False))

    # assert
    assert response.status == expected_data['status_code']
    assert json.loads(response.text) == expected_data['response']
