import aiohttp
import logging
from typing import Optional, Dict, Any, Tuple, List
import os

logger = logging.getLogger(__name__)

class ApiClient:
    def __init__(self):
        self.ai_proxy_url = os.getenv("AI_PROXY_URL", "http://localhost:8084")
        self.current_actions_url = os.getenv("CURRENT_ACTIONS_URL", "http://localhost:8080")
        self.sometime_later_url = os.getenv("SOMETIME_LATER_URL", "http://localhost:8081")
        self.waiting_url = os.getenv("WAITING_URL", "http://localhost:8082")
        self.notes_url = os.getenv("NOTES_URL", "http://localhost:8083")

    async def _request(self, method: str, url: str, data: Optional[Dict[str, Any]] = None) -> Tuple[Optional[Dict[str, Any]], int]:
        async with aiohttp.ClientSession() as session:
            try:
                async with session.request(method, url, json=data) as response:
                    logger.info(f"Request to {url} with data {data}. Status: {response.status}")
                    if response.status in [200, 201, 202]:
                        try:
                            return await response.json(), response.status
                        except aiohttp.ContentTypeError: # 201, 202 can return empty body
                            return {}, response.status
                    else:
                        error_text = await response.text()
                        logger.error(f"Error from {url}: {response.status} - {error_text}")
                        return {"error": error_text, "status_code": response.status}, response.status
            except aiohttp.ClientConnectorError as e:
                logger.error(f"Connection error to {url}: {e}")
                return {"error": str(e), "status_code": 503}, 503 # Service Unavailable

    # AI Proxy
    async def ai_define_task(self, text: str) -> Tuple[Optional[Dict[str, Any]], int]:
        url = f"{self.ai_proxy_url}/ai-proxy/v1/task/define"
        payload = {"task": text}
        return await self._request("POST", url, payload)

    # Common task methods
    async def _create_item(self, base_url: str, endpoint_suffix: str, user_id: int, name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        url = f"{base_url}/{endpoint_suffix}"
        payload = {"user_id": user_id, "name": name, "description": name} # name == description as per req
        return await self._request("POST", url, payload)

    async def _list_items(self, base_url: str, endpoint_suffix: str, user_id: int, status: Optional[str] = None, cursor: Optional[str] = None, limit: int = 10) -> Tuple[Optional[Dict[str, Any]], int]:
        url = f"{base_url}/{endpoint_suffix}"
        payload = {"user_id": user_id}
        if status:
            payload["status"] = status
        if cursor:
            payload["cursor"] = cursor
        # payload["limit"] = limit # Add limit if API supports it, good for pagination
        return await self._request("POST", url, payload)

    async def _update_item_name(self, base_url: str, endpoint_suffix: str, item_id: str, new_name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        url = f"{base_url}/{endpoint_suffix}"
        payload = {"task_id": item_id, "name": new_name, "description": new_name} # Assuming note_id if notes
        if "note" in endpoint_suffix: # API for notes uses note_id
             payload = {"note_id": item_id, "name": new_name, "description": new_name}
        return await self._request("POST", url, payload)

    async def _simple_item_action(self, base_url: str, endpoint_suffix: str, item_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        url = f"{base_url}/{endpoint_suffix}"
        payload = {"task_id": item_id}
        if "note" in endpoint_suffix: # API for notes uses note_id
            payload = {"note_id": item_id}
        return await self._request("POST", url, payload)

    # Current Actions
    async def create_current_action(self, user_id: int, name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._create_item(self.current_actions_url, "current-actions/v1/task", user_id, name)

    async def list_current_actions(self, user_id: int, status: Optional[str] = "active", cursor: Optional[str] = None) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._list_items(self.current_actions_url, "current-actions/v1/task/list", user_id, status, cursor)

    async def update_current_action_name(self, task_id: str, new_name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._update_item_name(self.current_actions_url, "current-actions/v1/task/update", task_id, new_name)

    async def complete_current_action(self, task_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._simple_item_action(self.current_actions_url, "current-actions/v1/task/complete", task_id)

    async def move_current_action_to_sometime_later(self, task_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._simple_item_action(self.current_actions_url, "current-actions/v1/task/sometime/later", task_id)

    async def remove_current_action(self, task_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._simple_item_action(self.current_actions_url, "current-actions/v1/task/remove", task_id)

    # Sometime/Later
    async def create_sometime_later_task(self, user_id: int, name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._create_item(self.sometime_later_url, "sometime-later/v1/task", user_id, name)

    async def list_sometime_later_tasks(self, user_id: int, status: Optional[str] = "pending", cursor: Optional[str] = None) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._list_items(self.sometime_later_url, "sometime-later/v1/task/list", user_id, status, cursor)

    async def update_sometime_later_task_name(self, task_id: str, new_name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._update_item_name(self.sometime_later_url, "sometime-later/v1/task/update", task_id, new_name)

    async def complete_sometime_later_task(self, task_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._simple_item_action(self.sometime_later_url, "sometime-later/v1/task/complete", task_id)

    async def move_sometime_later_to_current_actions(self, task_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._simple_item_action(self.sometime_later_url, "sometime-later/v1/task/current/actions", task_id)

    async def remove_sometime_later_task(self, task_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._simple_item_action(self.sometime_later_url, "sometime-later/v1/task/remove", task_id)

    # Waiting
    async def create_waiting_task(self, user_id: int, name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._create_item(self.waiting_url, "waiting/v1/task", user_id, name)

    async def list_waiting_tasks(self, user_id: int, status: Optional[str] = "active", cursor: Optional[str] = None) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._list_items(self.waiting_url, "waiting/v1/task/list", user_id, status, cursor)

    async def update_waiting_task_name(self, task_id: str, new_name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._update_item_name(self.waiting_url, "waiting/v1/task/update", task_id, new_name)

    async def complete_waiting_task(self, task_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._simple_item_action(self.waiting_url, "waiting/v1/task/complete", task_id)

    async def remove_waiting_task(self, task_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._simple_item_action(self.waiting_url, "waiting/v1/task/remove", task_id)

    # Notes
    async def create_note(self, user_id: int, name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._create_item(self.notes_url, "notes/v1/note", user_id, name)

    async def list_notes(self, user_id: int, status: Optional[str] = "active", cursor: Optional[str] = None) -> Tuple[Optional[Dict[str, Any]], int]:
        return await self._list_items(self.notes_url, "notes/v1/note/list", user_id, status, cursor)

    async def update_note_name(self, note_id: str, new_name: str) -> Tuple[Optional[Dict[str, Any]], int]:
        # Note specific 'note_id' in payload
        return await self._update_item_name(self.notes_url, "notes/v1/note/update", note_id, new_name)

    async def remove_note(self, note_id: str) -> Tuple[Optional[Dict[str, Any]], int]:
        # Note specific 'note_id' in payload
        return await self._simple_item_action(self.notes_url, "notes/v1/note/remove", note_id)

api_client = ApiClient()
