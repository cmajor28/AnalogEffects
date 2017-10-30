import Adafruit_BluefruitLE
from Adafruit_BluefruitLE.services import UART
import json
import threading
import time

class Remote:
    # Get the BLE provider for the current platform.
    ble = Adafruit_BluefruitLE.get_provider()

    def __init__(self, info, updateInfoCallback):
        self.updateInfoCallback = updateInfoCallback
        self.info = info
        self.info["id"] = None
        self.device = None
        self.adapter = None
        self.uart = None

        # Initialize the BLE system.  MUST be called before other BLE calls!
        Remote.ble.initialize()

        # The BLE loop will run in another thread
        threading.Thread(target=Remote.ble.run_mainloop_with, args=(self.__ble_loop,)).start()

    def updateInfo(self, info):
        if "id" not in info or info["id"] == None:
            if self.device != None:
                self.device.disconnect()
                self.info["id"] = None
        if self.uart != None:
            self.info = info
            self.uart.write(json.dumps({"bank":info["bank"]}, seperators=(',',':')))

    # Main function implements the program logic so it can run in a background
    # thread.  Most platforms require the main thread to handle GUI events and other
    # asyncronous events like BLE actions.  All of the threading logic is taken care
    # of automatically though and you just need to provide a main function that uses
    # the BLE provider.
    def __ble_loop(self):
        # Clear any cached data because both bluez and CoreBluetooth have issues with
        # caching data and it going stale.
        Remote.ble.clear_cached_data()

        # Get the first available BLE network adapter and make sure it's powered on.
        self.adapter = Remote.ble.get_default_adapter()
        self.adapter.power_on()
        print('Using adapter: {0}'.format(self.adapter.name))

        # Disconnect any currently connected UART devices.  Good for cleaning up and
        # starting from a fresh state.
        print('Disconnecting any connected UART devices...')
        UART.disconnect_devices()

        while (True):
            # Scan for UART devices.
            print('Searching for UART device...')
            while (True):
                self.adapter.start_scan()
                # Search for the first UART device found.
                self.device = UART.find_device(timeout_sec=50)
                self.adapter.stop_scan()
                if self.device is None:
                    time.sleep(10)
                    continue
                else:
                    break

            print('Connecting to device...')
            self.device.connect()  # Will time out after 60 seconds, specify timeout_sec parameter to change the timeout.

            if not self.device.is_connected():
                print('Failed to connect to device...')
                continue

            self.info["id"] = self.device.id

            # Once connected do everything else in a try/finally to make sure the device
            # is disconnected when done.
            try:
                # Wait for service discovery to complete for the UART service.  Will
                # time out after 60 seconds (specify timeout_sec parameter to override).
                print('Discovering services...')
                UART.discover(self.device)

                # Once service discovery is complete create an instance of the service
                # and start interacting with it.
                self.uart = UART(self.device)

                while self.device.is_connected():
                    received = self.uart.read(timeout_sec=10)
                    if received is not None:
                        # Received data, print it out.
                        print('Received: {0}'.format(received))

            finally:
                # Make sure device is disconnected on exit.
                print('Disconnecting from device...')
                self.device.disconnect()
                self.device = None
                self.info["id"] = None
