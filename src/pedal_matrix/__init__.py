from flask import Flask, request
from flask_bootstrap import Bootstrap
import os
import time
import signal
from ctypes import *
from bluetooth.remote import *
from gui.lcd import *
import threading
import socket
import fcntl
import struct
import json

app = Flask(__name__)
Bootstrap(app)

c_lib = cdll.LoadLibrary("./aeffects.so")  # call construct at startup, def __init (in essence, calling C constructor)

class AE_PRESET(Structure):
    _fields_ = [("bank", c_int),
                ("preset", c_int),
                ("pedalOrder", c_int * 7),
                ("enabled", c_bool * 7),
                ("controlEnabled", c_bool * 2)]

class AE_CALLBACKS(Structure):
    _fields_ = [("presetChanged", CFUNCTYPE(c_int, c_int)),
                ("bankChanged", CFUNCTYPE(c_int, c_int)),
                ("modeChanged", CFUNCTYPE(c_int, c_bool)),
                ("bypassEnabled", CFUNCTYPE(c_int, c_bool)),
                ("muteEnabled", CFUNCTYPE(c_int, c_bool))]


# These are created later
remote = None
lcd = None

# Used for qt
guiInvoker = None

remoteInfo = {"id": "",
              "bank": -1,
              "preset": -1}

lcdInfo = {"bank": -1,
           "preset": -1,
           "presetName": "",
           "webAddress": "",
           "remoteID": "",
           "pedalMode": False,
           "bypassMode": False,
           "muteMode": False,
           "webEnabled": False,
           "remotePaired": False}

"""@app.route('/')
def index():
    return render_template('index.html')"""

@app.route('/Pedal_Inventory', methods=['POST'])
def pedal_inventory():
    pedal_name = request.json['pedal_name']
    pedal_num = request.json['pedal_num']
    pedal_descrip = request.json['pedal_descrip']
    pedal_sett = request.json['pedal_sett']

    data = {
        'pedal_name': pedal_name,
        'pedal_num': pedal_num,
        'pedal_descrip': pedal_descrip,
        'pedal_sett': pedal_sett
    }

    # Write to JSON
    with open('pedal_inventory.json', 'w') as f:
        json.dump(data, f)

    return "Success"


@app.route('/', methods=['GET', 'POST'])
def preset_order():
    if request.method == 'GET':
        return """
<!DOCTYPE html>
<html lang="en-US">
<body>
<form method="POST">
    <p>Control Enabled1:<br>
        <select name="controlEnabled1">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
        </select>
    </p>

    <p>Control Enabled2:<br>
        <select name="controlEnabled2">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
        </select>
    </p>

    <p>
        Bank Name:
        <input name="bank_name" type="text" size="25">
    </p>

    <p>
        Bank Number:
        <input name="bank_num" type="text" size="25">
    </p>

    <p>
        Preset Name:
        <input name="preset_name" type="text" size="25">
    </p>

    <p>
        Preset Number:
        <input name="preset_num" type="text" size="25">
    </p>

    <p>Pedal 1:<br>
        <select name="pedal_pos1">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
          <option value="6">6</option>
          <option value="7">7</option>
        </select>
    </p>

    <p>Pedal 2:<br>
        <select name="pedal_pos2">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
          <option value="6">6</option>
          <option value="7">7</option>
        </select>
    </p>

    <p>Pedal 3:<br>
        <select name="pedal_pos3">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
          <option value="6">6</option>
          <option value="7">7</option>
        </select>
    </p>

    <p>Pedal 4:<br>
        <select name="pedal_pos4">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
          <option value="6">6</option>
          <option value="7">7</option>
        </select>
    </p>

    <p>Pedal 5:<br>
        <select name="pedal_pos5">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
          <option value="6">6</option>
          <option value="7">7</option>
        </select>
    </p>

    <p>Pedal 6:<br>
        <select name="pedal_pos6">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
          <option value="6">6</option>
          <option value="7">7</option>
        </select>
    </p>

    <p>Pedal 7:<br>
        <select name="pedal_pos7">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
          <option value="6">6</option>
          <option value="7">7</option>
        </select>
    </p>

    <p>Pedal 1 Enabled:<br>
        <select name="enabled_pos1">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
        </select>
    </p>

    <p>Pedal 2 Enabled:<br>
        <select name="enabled_pos2">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
        </select>
    </p>

    <p>Pedal 3 Enabled:<br>
        <select name="enabled_pos3">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
        </select>
    </p>

    <p>Pedal 4 Enabled:<br>
        <select name="enabled_pos4">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
        </select>
    </p>

    <p>Pedal 5 Enabled:<br>
        <select name="enabled_pos5">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
        </select>
    </p>

    <p>Pedal 6 Enabled:<br>
        <select name="enabled_pos6">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
        </select>
    </p>

    <p>Pedal 7 Enabled:<br>
        <select name="enabled_pos7">
          <option value="0" selected="selected">0</option>
          <option value="1">1</option>
        </select>
    </p>

    <p>
        <input type="submit" value="Submit">
    </p>
</form>
</body>
</html>
    """
    else:
        pedal = [0, 0, 0, 0, 0, 0, 0]
        enable = [0, 0, 0, 0, 0, 0, 0]
        controlEnabled = [0, 0]

        controlEnabled[0] = request.form['controlEnabled1']
        controlEnabled[1] = request.form['controlEnabled2']

        bank_name = request.form['bank_name']
        bank_num = int(request.form['bank_num'])
        preset_name = request.form['preset_name']
        preset_num = int(request.form['preset_num'])


        #pedal_pos1_name = request.json['pedal_pos1_name']
        #pedal_pos2_name = request.json['pedal_pos2_name']
        #pedal_pos3_name = request.json['pedal_pos3_name']
        #pedal_pos4_name = request.json['pedal_pos4_name']
        #pedal_pos5_name = request.json['pedal_pos5_name']
        #pedal_pos6_name = request.json['pedal_pos6_name']
        #pedal_pos7_name = request.json['pedal_pos7_name']

        pedal[0] = int(request.form['pedal_pos1'])
        pedal[2] = int(request.form['pedal_pos3'])
        pedal[1] = int(request.form['pedal_pos2'])
        pedal[3] = int(request.form['pedal_pos4'])
        pedal[4] = int(request.form['pedal_pos5'])
        pedal[5] = int(request.form['pedal_pos6'])
        pedal[6] = int(request.form['pedal_pos7'])

        enable[0] = int(request.form['enabled_pos1'])
        enable[1] = int(request.form['enabled_pos2'])
        enable[2] = int(request.form['enabled_pos3'])
        enable[3] = int(request.form['enabled_pos4'])
        enable[4] = int(request.form['enabled_pos5'])
        enable[5] = int(request.form['enabled_pos6'])
        enable[6] = int(request.form['enabled_pos7'])

        new_preset = AE_PRESET()
        new_preset.preset = preset_num
        new_preset.bank = bank_num
        new_preset.pedalOrder = (c_int * 7)(*pedal)
        new_preset.enabled = (c_bool * 7)(*enable)
        new_preset.controlEnabled = (c_bool * 2)(*controlEnabled)

        c_lib.aeffects_update(byref(new_preset))

        # 0 indicates unused pedal, 8 indicates final output
        data = {
            'controlEnabled1': controlEnabled[0],
            'controlEnabled2': controlEnabled[1],
            'bank_name': bank_name,
            'bank_num': bank_num,
            'preset_name': preset_name,
            'preset_num': preset_num,
            #"pedal_pos1_name": pedal_pos1_name,
            #"pedal_pos2_name": pedal_pos2_name,
            #"pedal_pos3_name": pedal_pos3_name,
            #"pedal_pos4_name": pedal_pos4_name,
            #"pedal_pos5_name": pedal_pos5_name,
            #"pedal_pos6_name": pedal_pos6_name,
            #"pedal_pos7_name": pedal_pos7_name,
            'pedal_pos1': pedal[0],
            'pedal_pos2': pedal[1],
            'pedal_pos3': pedal[2],
            'pedal_pos4': pedal[3],
            'pedal_pos5': pedal[4],
            'pedal_pos6': pedal[5],
            'pedal_pos7': pedal[6],
            "enabled_pos1": enable[0],
            "enabled_pos2": enable[1],
            "enabled_pos3": enable[2],
            "enabled_pos4": enable[3],
            "enabled_pos5": enable[4],
            "enabled_pos6": enable[5],
            "enabled_pos7": enable[6]
        }

        # Write to JSON
        filename = "%d_%d" % (bank_num, preset_num)
        with open('presets_' + filename + '.json', 'w') as f:
            json.dump(data, f)

        # Read JSON
        """with open('data.json', 'r') as f:
            data = json.load(f)"""

        return "Success"


# add controlEnabled bool array for init function
def init_c_lib():
    new_preset = (16 * 8 * AE_PRESET)()
    for bank_num in range(0, 16):
        for preset_num in range(0, 8):
            read_pedal = [0, 0, 0, 0, 0, 0, 0]
            read_enable = [0, 0, 0, 0, 0, 0, 0]
            new_preset[bank_num * 8 + preset_num].preset = bank_num
            new_preset[bank_num * 8 + preset_num].bank = preset_num

            filename = "%d_%d" % (bank_num + 1, preset_num + 1)
            if not os.path.isfile('presets_' + filename + '.json'):
                break

            with open('presets_' + filename + '.json', 'r') as f:
                data = json.load(f)

            for i in range(1, 8):
                read_enable[i - 1] = data['enabled_pos' + str(i)]
                read_pedal[i - 1] = data['pedal_pos' + str(i)]
            new_preset[bank_num * 8 + preset_num].pedalOrder = (c_int * 7)(*read_pedal)
            new_preset[bank_num * 8 + preset_num].enabled = (c_bool * 7)(*read_enable)

    c_lib.aeffects_init(new_preset)  # convert 2d tuple of presets to 2d array passed through init
    return


def set_bank_c(bank):
    c_lib.set_bank(c_int(bank))
    return


def set_preset_c(preset):
    c_lib.set_preset(c_int(preset))
    return


def set_mode_c(mode):
    c_lib.set_mode(c_bool(mode))
    return


def set_bypass_c(bypass):
    c_lib.set_bypass(c_bool(bypass))
    return


def set_mute_c(mute):
    c_lib.set_mute(c_bool(mute))
    return


def set_bank_py(bank):
    global lcd, remote, lcdInfo, remoteInfo
    remoteInfo["bank"] = c_int(bank).value
    lcdInfo["bank"] = c_int(bank).value
    if remote is not None:
        remote.updateInfo(remoteInfo)
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, lcdInfo)
    return c_int(0)


def set_preset_py(preset):
    global lcd, remote, lcdInfo, remoteInfo
    lcdInfo["preset"] = c_int(preset).value
    remoteInfo["preset"] = c_int(preset).value

    bank = c_int(0)
    c_lib.get_bank(byref(bank))

    # read from file for preset name
    filename = "%d_%d" % (bank.value, c_int(preset).value)
    presetName = ''
    if os.path.isfile('presets_' + filename + '.json'):
        with open('presets_' + filename + '.json', 'r') as f:
            data = json.load(f)
            presetName = data['preset_name']
	
    lcdInfo["presetName"] = presetName
	
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, lcdInfo)
    return c_int(0)


def set_mode_py(mode):
    global lcd, remote, lcdInfo, remoteInfo
    lcdInfo["mode"] = c_bool(mode).value
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, lcdInfo)
    return c_int(0)


def set_bypass_py(bypass):
    global lcd, remote, lcdInfo, remoteInfo
    lcdInfo["bypass"] = c_bool(bypass).value
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, lcdInfo)
    return c_int(0)


def set_mute_py(mute):
    global lcd, remote, lcdInfo, remoteInfo
    lcdInfo["mute"] = c_bool(mute).value
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, lcdInfo)
    return c_int(0)


def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        ip = socket.inet_ntoa(fcntl.ioctl(
            s.fileno(),
            0x8915,  # SIOCGIFADDR
            struct.pack('256s', bytes(ifname[:15], 'utf-8'))
        )[20:24])
        ip = ip + ":5052"
    except:
        ip = ''
    finally:
        return ip


def update_ip_address():
    global lcdInfo
    while True:
        ip = get_ip_address("usb0")
        if ip != lcdInfo["webAddress"]:
            print("Web Address Changed: '" + ip + "'")
            lcdInfo["webAddress"] = ip
            if lcd is not None:
                guiInvoker.invoke(lcd.updateInfo, lcdInfo)
        time.sleep(5)


def init_structs():
    global lcdInfo, remoteInfo
    bank = c_int(0)
    c_lib.get_bank(byref(bank))
    preset = c_int(0)
    c_lib.get_preset(byref(preset))
    mode = c_bool(False)
    c_lib.get_mode(byref(mode))
    bypass = c_bool(False)
    c_lib.get_bypass(byref(bypass))
    mute = c_bool(False)
    c_lib.get_mute(byref(mute))
    lcdInfo["bank"] = remoteInfo["bank"] = bank.value
    lcdInfo["preset"] = preset.value

    # read from file for preset name
    filename = "%d_%d" % (bank.value, preset.value)
    presetName = ''
    if os.path.isfile('presets_' + filename + '.json'):
        with open('presets_' + filename + '.json', 'r') as f:
            data = json.load(f)
            presetName = data['preset_name']
	
    lcdInfo["presetName"] = presetName
    lcdInfo["webAddress"] = get_ip_address("wlan0")
    lcdInfo["remoteID"] = None
    lcdInfo["pedalMode"] = mode.value
    lcdInfo["bypassMode"] = bypass.value
    lcdInfo["muteMode"] = mute.value
    lcdInfo["webEnabled"] = True
    lcdInfo["remotePaired"] = False

    print("Web Address: '" + lcdInfo["webAddress"] + "'")

    threading.Thread(target=update_ip_address).start()


def init_control():
    callbacks = AE_PRESET()

    c_int_arg_func = CFUNCTYPE(c_int, c_int)
    c_bool_arg_func = CFUNCTYPE(c_int, c_bool)

    callbacks.presetChanged = c_int_arg_func(set_preset_py)
    callbacks.bankChanged = c_int_arg_func(set_bank_py)
    callbacks.modeChanged = c_bool_arg_func(set_mode_py)
    callbacks.bypassEnabled = c_bool_arg_func(set_bypass_py)
    callbacks.muteEnabled = c_bool_arg_func(set_mute_py)

    c_lib.register_callbacks(byref(callbacks))


def lcdUpdate(info):
    global lcd, remote, lcdInfo, remoteInfo
    if info["pedalMode"] != lcdInfo["pedalMode"]:
        set_mode_c(info["pedalMode"])
    if info["bypassMode"] != lcdInfo["bypassMode"]:
        set_bypass_c(info["bypassMode"])
    if info["muteMode"] != lcdInfo["muteMode"]:
        set_mute_c(info["muteMode"])
    if info["webEnabled"] != lcdInfo["webEnabled"]:
        if info["webEnabled"]:
            print("Enabling wlan0...")
            os.system("ifconfig wlan0 up")
        else:
            print("Disabling wlan0...")
            os.system("ifconfig wlan0 down")
    if info["remotePaired"] != lcdInfo["remotePaired"]:
        remoteInfo["id"] = None
        remote.updateInfo(remoteInfo)
    lcdInfo = info.copy()


def remoteUpdate(info):
    global lcd, remote, lcdInfo, remoteInfo
    if info["bank"] != remoteInfo["bank"]:
        lcdInfo["bank"] = info["bank"]
        set_bank_c(info["bank"])
    if info["preset"] != remoteInfo["preset"]:
        lcdInfo["preset"] = info["preset"]
        set_preset_c(info["preset"])
    if info["id"] != remoteInfo["id"]:
        lcdInfo["remoteID"] = info["id"]
        lcdInfo["remotePaired"] = (info["id"] != None)
    guiInvoker.invoke(lcd.updateInfo, lcdInfo)
    remoteInfo = info.copy()


def run_gui():
    global lcd, guiInvoker
    app = QApplication(sys.argv)
    #signal.signal(signal.SIGINT, signal.SIG_DFL)
    lcd = LCDWindow(lcdInfo.copy(), lcdUpdate)
    lcd.show()
    guiInvoker = Invoker()
    sys.exit(app.exec_())

def run_app():
    #app.run(host='0.0.0.0', port=5052, debug=True)
    app.run(host='0.0.0.0', port=5052)


if __name__ == "__main__":
    init_c_lib()
    init_control()
    init_structs()
    remote = Remote(remoteInfo.copy(), remoteUpdate)
    threading.Thread(target=run_gui).start()
    run_app()