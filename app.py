from flask import Flask, render_template, request
import requests

# Replace with your ThingSpeak channel ID and API read key
channel_id = 2487047  # Replace with your channel ID
api_key = "QJZ3ELIZ7VFB0NFP"

app = Flask(__name__)


@app.route("/")
def index():
    free_slots = get_free_slots()  # Call function to fetch data
    which_slot = int(get_which_slot())
    if which_slot == 0:
        display_slot = "Slot-1 and Slot-2 are Free"
    elif which_slot == 1:
        display_slot = "Slot-1 is Free"
    else:
        display_slot = "Slot-2 is Free"
    return render_template(
        "index.html", free_slots=free_slots, display_slot=display_slot
    )


def get_which_slot():
    base_url = f"https://api.thingspeak.com/channels/{channel_id}/feeds.json"
    params = {"api_key": api_key, "results": 1}

    try:
        response = requests.get(base_url, params=params)
        if response.status_code == 200:
            data = response.json()
            return data["feeds"][0]["field2"]
        else:
            return "Error fetching data"
    except requests.exceptions.RequestException as e:
        return f"Error: {e}"


def get_free_slots():

    base_url = f"https://api.thingspeak.com/channels/{channel_id}/feeds.json"
    params = {"api_key": api_key, "results": 1}

    try:
        response = requests.get(base_url, params=params)
        if response.status_code == 200:
            data = response.json()
            return data["feeds"][0]["field1"]
        else:
            return "Error fetching data"
    except requests.exceptions.RequestException as e:
        return f"Error: {e}"


if __name__ == "__main__":
    app.run(debug=True)
