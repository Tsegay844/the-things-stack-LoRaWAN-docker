from requests.auth import HTTPBasicAuth
import requests


def retrieve_stored_uplinks(key_id, api_key, limit=10, after="2024-07-02T00:00:00Z"):
    url = f"https://eu1.cloud.thethings.network/api/v3/as/applications/nucleo-wl55jc1-sht30/packages/storage/uplink_message"
    headers = {
        "Accept": "text/event-stream"
    }
    params = {
        "limit": limit,
        "after": after
    }
    auth = HTTPBasicAuth(key_id, api_key)
    response = requests.get(url, headers=headers, auth=auth, params=params)

    if response.status_code == 200:
        return response.text
    else:
        response.raise_for_status()


api_key = "NNSXS.XTHLU7MNY7KOYUBN7AKPIOLXLVJSVZRAGISGZHQ.HFAO56MQ34N4AEXPWH5OG7UAFVP3Q5F65K7NORKLCDOHEL4P3IDA"
key_id = "XTHLU7MNY7KOYUBN7AKPIOLXLVJSVZRAGISGZHQ"
uplinks = retrieve_stored_uplinks(key_id, api_key)
print(uplinks)
