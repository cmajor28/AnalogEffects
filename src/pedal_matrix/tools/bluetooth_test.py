# Example of interaction with a BLE UART device using a UART service
# implementation.
# Author: Tony DiCola
import Adafruit_BluefruitLE
from Adafruit_BluefruitLE.services import UART
import time

# Get the BLE provider for the current platform.
ble = Adafruit_BluefruitLE.get_provider()


# Main function implements the program logic so it can run in a background
# thread.  Most platforms require the main thread to handle GUI events and other
# asyncronous events like BLE actions.  All of the threading logic is taken care
# of automatically though and you just need to provide a main function that uses
# the BLE provider.
def main():

    # Clear any cached data because both bluez and CoreBluetooth have issues with
    # caching data and it going stale.
    print('Clearing Bluetooth cache...')
    ble.clear_cached_data()

    # Get the first available BLE network adapter and make sure it's powered on.
    adapter = ble.get_default_adapter()
    adapter.power_on()
    print('Using adapter: {0}'.format(adapter.name))

    # Disconnect any currently connected UART devices.  Good for cleaning up and
    # starting from a fresh state.
    print('Disconnecting any connected UART devices...')
    UART.disconnect_devices()

    while True:
        # Scan for UART devices.
        print('Searching for UART device...')
        while True:
            try:
                adapter.start_scan(timeout_sec=30)
            except:
                print("Adapter scan failed...")
                time.sleep(10)
                continue

            # Search for the first UART device found.
            try:
                device = UART.find_device(timeout_sec=50)
            except:
                pass
            adapter.stop_scan()
            if device is None:
                time.sleep(10)
                continue
            else:
                print("Found device {0} '{1}'".format(device.id, device.name))
                break

        print('Connecting to device...')

        try:
            device.connect(timeout_sec=5)  # Will time out after 60 seconds, specify timeout_sec parameter to change the timeout.
        except Exception as inst:
            print(inst)

        if not device.is_connected:
            print('Failed to connect to device...')
            continue

        # Once connected do everything else in a try/finally to make sure the device
        # is disconnected when done.
        try:
            # Wait for service discovery to complete for the UART service.  Will
            # time out after 60 seconds (specify timeout_sec parameter to override).
            print('Discovering services...')
            UART.discover(device, timeout_sec=10)

            print('Creating UART service...')

            # Once service discovery is complete create an instance of the service
            # and start interacting with it.
            uart = UART(device)

            print('Waiting for input...')

            while device.is_connected:
                received = uart.read(timeout_sec=5)
                if received is not None:
                    # Received data, print it out.
                    print('Received: {0}'.format(received))

        except Exception as inst:
            print(inst)

        finally:
            # Make sure device is disconnected on exit.
            print('Disconnecting from device...')
            try:
                device.disconnect(timeout_sec=5)
            except:
                pass


# Initialize the BLE system.  MUST be called before other BLE calls!
ble.initialize()

# Start the mainloop to process BLE events, and run the provided function in
# a background thread.  When the provided main function stops running, returns
# an integer status code, or throws an error the program will exit.
ble.run_mainloop_with(main)