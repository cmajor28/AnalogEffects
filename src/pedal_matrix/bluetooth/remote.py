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
        self.buffer = ''

        # Initialize the BLE system.  MUST be called before other BLE calls!
        Remote.ble.initialize()

        # The BLE loop will run in another thread
        threading.Thread(target=Remote.ble.run_mainloop_with, args=(self.__ble_loop,)).start()

    def updateInfo(self, info):
        # TODO add way to un-pair
        self.info = info
        if self.uart != None:
            print("Updating remote...")
            self.uart.write(bytes(json.dumps({"bank": self.info["bank"]}, separators=(',', ':')), 'utf-8'))

    def __parse_message(self, received):
        self.buffer += received
        index = -1
        for i in range(0, len(self.buffer)):
            if self.buffer[i] == '{':
                # Found start of msg
                index = i
            elif index != -1 and self.buffer[i] == '}':
                # Found end of msg
                result = self.buffer[index:i+1]
                self.buffer = self.buffer[i+1:]
                return result
        return ''

    # Main function implements the program logic so it can run in a background
    # thread.  Most platforms require the main thread to handle GUI events and other
    # asyncronous events like BLE actions.  All of the threading logic is taken care
    # of automatically though and you just need to provide a main function that uses
    # the BLE provider.
    def __ble_loop(self):
        error = False # Keep up with error case

        # Clear any cached data because both bluez and CoreBluetooth have issues with
        # caching data and it going stale.
        print('Clearing bluetooth cache...')
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
            if error:
                error = False
                print('Clearing bluetooth cache...')
                Remote.ble.clear_cached_data()

            # Scan for UART devices.
            print('Searching for UART device...')
            while (True):
                try:
                    self.adapter.start_scan(timeout_sec=30)
                except:
                    print("Adapter scan failed...")
                    time.sleep(10)
                    continue

                # Search for the first UART device found.
                try:
                    self.device = UART.find_device(timeout_sec=50)
                except:
                    pass
                self.adapter.stop_scan()
                if self.device is None:
                    time.sleep(10)
                    continue
                else:
                    print("Found device {0} '{1}'".format(self.device.id, self.device.name))
                    break

            print('Connecting to device...')

            try:
                self.device.connect(timeout_sec=10)  # Will time out after 60 seconds, specify timeout_sec parameter to change the timeout.
            except Exception as inst:
                print(inst)
                error = True

            if not self.device.is_connected:
                print('Failed to connect to device...')
                continue

            self.info["id"] = self.device.id
            self.updateInfoCallback(self.info)

            # Once connected do everything else in a try/finally to make sure the device
            # is disconnected when done.
            try:
                # Wait for service discovery to complete for the UART service.  Will
                # time out after 60 seconds (specify timeout_sec parameter to override).
                print('Discovering services...')
                UART.discover(self.device, timeout_sec=5)

                # Once service discovery is complete create an instance of the service
                # and start interacting with it.
                print('Creating UART device...')
                self.uart = UART(self.device)


                self.info["paired"] = True
                self.updateInfoCallback(self.info)

                if self.device.is_connected:
                    print("Updating remote...")
                    self.uart.write(bytes(json.dumps({"bank":self.info["bank"]}, separators=(',', ':')), 'utf-8'))

                    print('Waiting for UART input...')

                    while self.device.is_connected:
                        received = self.uart.read(timeout_sec=5)
                        if received is not None:
                            # Received data, print it out.
                            print('Received: {0}'.format(received))

                            while True:
                                buffer = self.__parse_message(received)
                                if buffer != '':
                                    received = ''
                                    try:
                                        msg = json.loads(buffer)
                                    except Exception as inst:
                                        print(inst)
                                    if "preset" in msg:
                                        self.info["preset"] = int(msg["preset"])
                                    if "bank" in  msg:
                                        self.info["bank"] = int(msg["bank"])
                                    self.updateInfoCallback(self.info)
                                else:
                                    break

            except Exception as inst:
                print(inst)
                error = True

            finally:
                # Make sure device is disconnected on exit.
                print('Disconnecting from device...')
                try:
                    self.device.disconnect(timeout_sec=5)
                except Exception as inst:
                    print(inst)
                    error = True
                self.device = None
                self.info["id"] = None
                self.info["paired"] = False
                self.updateInfoCallback(self.info)
