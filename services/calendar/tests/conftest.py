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
        service_source_dir / 'tests/test_calendar/static/test_create_note/testcases',
        service_source_dir / 'tests/test_calendar/static/test_complete_note/testcases',
        service_source_dir / 'tests/test_calendar/static/test_reactivate_note/testcases',
    ]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'calendar',  # service name that goes to the DB connection
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))
