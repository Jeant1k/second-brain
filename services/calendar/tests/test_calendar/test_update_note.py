import pytest
import json

from testsuite.databases import pgsql
from .plugins.utils import select_note
from .plugins.constants import NOTE_ID


@pytest.mark.parametrize(
    'testcase',
    [
        pytest.param('empty_note_id'),
        pytest.param('empty_name_and_description'),
        pytest.param('note_id_wrong_format'),
        pytest.param('note_not_found'),
        pytest.param('happy_path_by_name'),
        pytest.param('happy_path_by_description'),
        pytest.param('happy_path_full'),
    ],
)
@pytest.mark.pgsql('calendar', files=['database.sql'])
async def test_update_note(
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
        '/calendar/v1/note/update',
        json=initial_data['request_body'],
    )

    print(json.dumps(response.json(), indent=2, ensure_ascii=False))
    print(select_note(pgsql, NOTE_ID))

    # assert
    assert response.status == expected_data['status_code']
    assert json.loads(response.text) == expected_data['response']

    assert select_note(pgsql, NOTE_ID) == expected_data['database_data']
