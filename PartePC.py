import paho.mqtt.client as mqtt
import subprocess
import os

# --- MQTT Configuration ---
MQTT_BROKER = "test.mosquitto.org"  # broker address
MQTT_PORT = 1883
MQTT_TOPIC = "teste/MovingMusic/zonas" # Topic to subscribe to for commands
#MQTT_USERNAME = "your_username"  # Optional
#MQTT_PASSWORD = "your_password"  # Optional

# --- Audio Device Configuration ---
# IMPORTANT: Replace these with the exact names of your audio devices
# You can find these in Windows Sound settings (right-click speaker icon in taskbar -> Sound settings)
NOME_COMMODO_1 = "Sala"
DEVICE_COMMODO_1 = "Altofalantes"
NOME_COMMODO_2 = "Quarto"
DEVICE_COMMODO_2 = "Alto-falantes"
# Path to SoundVolumeView.exe (download from NirSoft and place it somewhere accessible)
SOUNDVOLUMEVIEW_PATH = "C:\\Program Files\\SoundVolumeView\\SoundVolumeView.exe"

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    print(f"Message received: {msg.topic} - {msg.payload.decode()}")
    command = msg.payload.decode().strip().lower()

    if command == NOME_COMMODO_1.lower():
        set_default_audio_device(DEVICE_COMMODO_1)
    elif command == NOME_COMMODO_2.lower():
        set_default_audio_device(DEVICE_COMMODO_2)
    else:
        print(f"Unknown command: {command}")

def set_default_audio_device(device_name):
    """
    Sets the default audio playback device using SoundVolumeView.
    """
    if not os.path.exists(SOUNDVOLUMEVIEW_PATH):
        print(f"Error: SoundVolumeView.exe not found at {SOUNDVOLUMEVIEW_PATH}")
        return

    try:
        # Command to set default playback device (0 for Console, 1 for Multimedia, 2 for Communications)
        cmd = [SOUNDVOLUMEVIEW_PATH, "/SetDefault", device_name, "0"]
        
        # Use subprocess.run for better control and error handling
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        print(f"Successfully set default audio device to: {device_name}")
        if result.stdout:
            print(f"SoundVolumeView Output: {result.stdout}")
        if result.stderr:
            print(f"SoundVolumeView Errors: {result.stderr}")
    except subprocess.CalledProcessError as e:
        print(f"Error changing audio device: {e}")
        print(f"Command failed with output:\n{e.stdout}\n{e.stderr}")
    except FileNotFoundError:
        print(f"Error: SoundVolumeView.exe not found. Make sure the path is correct: {SOUNDVOLUMEVIEW_PATH}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    # Initialize MQTT client
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    '''
    if MQTT_USERNAME and MQTT_PASSWORD:
        client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    '''
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_forever()
    except Exception as e:
        print(f"Failed to connect to MQTT broker: {e}")
    