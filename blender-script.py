import sys
import bpy
sys.path.append(bpy.path.abspath("//venv/lib/python3.12/site-packages"))
import serial
import time


#Remove previous scripts
bpy.app.handlers.frame_change_pre.clear()

# selecting a usb port, remeber to so: sudo chmod a+rw /dev/ttyUSB1

# Set up the serial connection to the Arduino
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)  # Adjust port as necessary
time.sleep(2)  # Allow time for the serial connection to establish

#while True:
#    if ser.in_waiting > 0:
#        data = ser.readline().decode('utf-8').strip()
#        print(f"Received: {data}")
#    else:
#        print("No serial data available.")
#    time.sleep(1)


# Define how many frames to wait before updating (e.g., update every 10 frames)
update_every_frames = 1
frame_counter = 0  # Initialize frame counter

def update_led_colors():
    print("update")
    # Read the serial data
    if ser.in_waiting > 0:
        print("serial waiting")
        led_data = ser.readline().decode('utf-8').strip()  # Read and decode the serial data
        if len(led_data) > 0:
            led_values = led_data.split('|')  # Split by '|' to get the RGB values of each LED
            # Update light colors in Blender
            for i in range(50):  # Assuming 3 LEDs
                r, g, b = map(int, led_values[i].split(','))  # Extract RGB values
                r_norm = r / 255.0
                g_norm = g / 255.0
                b_norm = b / 255.0

                # Set the color of the corresponding light in Blender
                light_obj = bpy.data.objects[f"led.{str(i).zfill(3)}"]  # 'Light.001', 'Light.002', 'Light.003'
                light_obj.data.color = (r_norm, g_norm, b_norm)  # Set the RGB color of the light
                
    else:
        print("no serial waiting")

# Run the update function every X frames
def frame_change_handler(scene):
    global frame_counter
    frame_counter += 1  # Increment the frame counter
    print("fish")

    # Only update the lights every X frames
    if frame_counter >= update_every_frames:
        update_led_colors()
        frame_counter = 0  # Reset the counter after updating

# Add the handler to execute the script on every frame change
bpy.app.handlers.frame_change_pre.append(frame_change_handler)
