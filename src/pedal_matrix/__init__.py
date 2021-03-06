from flask import Flask, request
from flask_bootstrap import Bootstrap
import os
from copy import deepcopy
import time
import signal
from ctypes import *
from bluetooth.remote import *
from gui.lcd import *
from string import Template
import threading
import socket
import fcntl
import struct
import json

app = Flask(__name__)
Bootstrap(app)
app.static_folder = 'static'
c_lib = cdll.LoadLibrary("./aeffects.so")  # call construct at startup, def __init (in essence, calling C constructor)

STATIC_SUCCESS_TEMPLATE = Template("""<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8" />
	<title>Pedal Matrix</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
</head>
<body class="blurBg-true" style="background-color:#EBEBEB">
<link rel="stylesheet" href="test1_files/formoid1/formoid-biz-red.css" type="text/css" />
<script type="text/javascript" src="test1_files/formoid1/jquery.min.js"></script>
<a href="${homepage}">Go Back</a>
</body>
</html>""")

class AE_PRESET(Structure):
    _fields_ = [("bank", c_int),
                ("preset", c_int),
                ("pedalOrder", c_int * 7),
                ("enabled", c_bool * 7),
                ("controlEnabled", c_bool * 2)]

# These are created later
remote = None
lcd = None

# Used for qt
guiInvoker = None

remoteInfo = {"id": "",
              "paired": False,
              "bank": -1,
              "preset": -1}

lcdInfo = {"bank": -1,
           "bankName": "",
           "preset": -1,
           "presetName": "",
           "webAddress": "",
           "remoteID": "",
           "pedalMode": False,
           "bypassMode": False,
           "muteMode": False,
           "webEnabled": False,
           "remotePaired": False,
           "pedals": [0, 0, 0, 0, 0, 0, 0],
           "enabled": [False, False, False, False, False, False, False],
           "presence": [False, False, False, False, False, False, False, False, False],
           "control": [False, False]}

# Used for ctypes
set_preset_py_func = None
set_bank_py_func = None
set_mode_py_func = None
set_bypass_py_func = None
set_mute_py_func = None
set_pedals_py_func = None
set_preset_py_func_type = None
set_bank_py_func_type = None
set_mode_py_func_type = None
set_bypass_py_func_type = None
set_mute_py_func_type = None
set_pedals_py_func_type = None

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
        return app.send_static_file('test1.html')
    
    else:
        pedal = [0, 0, 0, 0, 0, 0, 0]
        enable = [False, False, False, False, False, False, False]
        controlEnabled = [False, False]
        data = request.form
        bank_name = data['bank_name']
        bank_num = int(data['bank_num'])
        preset_name = data['preset_name']
        preset_num = int(data['preset_num'])
        #pedal_pos1_name = request.json['pedal_pos1_name']
        #pedal_pos2_name = request.json['pedal_pos2_name']
        #pedal_pos3_name = request.json['pedal_pos3_name']
        #pedal_pos4_name = request.json['pedal_pos4_name']
        #pedal_pos5_name = request.json['pedal_pos5_name']
        #pedal_pos6_name = request.json['pedal_pos6_name']
        #pedal_pos7_name = request.json['pedal_pos7_name']

        pedal[0] = int(data['pedal_pos1'])
        pedal[1] = int(data['pedal_pos2'])
        pedal[2] = int(data['pedal_pos3'])
        pedal[3] = int(data['pedal_pos4'])
        pedal[4] = int(data['pedal_pos5'])
        pedal[5] = int(data['pedal_pos6'])
        pedal[6] = int(data['pedal_pos7'])

        for i in range(1,8):
            curr = int(data['pedal_pos' + str(i)])
            if curr > 0:
                enable[curr-1] = 'enabled_pos' + str(i) in data

        controlEnabled[1] = 'controlEnabled1' in data
        controlEnabled[0] = 'controlEnabled2' in data
        
        # added to prevent repeat entries on pedal order
        index = [0,1,2,3,4,5,6]
        y = list(set([x for x in pedal if pedal.count(x) > 0]))
        z = []
        for i in range(0,len(y)):
            z.append(pedal.index(y[i]))
        j = list(set(index).difference(z))
        for p in range(0,len(j)):
            pedal[j[p]] = 0

        new_preset = AE_PRESET()
        new_preset.preset = preset_num - 1
        new_preset.bank = bank_num - 1
        new_preset.pedalOrder = (c_int * 7)(*pedal)
        new_preset.enabled = (c_bool * 7)(*enable)
        new_preset.controlEnabled = (c_bool * 2)(*controlEnabled)
        c_lib.aeffects_update(byref(new_preset))

        # 0 indicates unused pedal, 8 indicates final output
        data = {
            'controlEnabled1': controlEnabled[1],
            'controlEnabled2': controlEnabled[0],
            'bank_name': bank_name,
            'bank_num': bank_num - 1,
            'preset_name': preset_name,
            'preset_num': preset_num - 1,
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

        with open('data/preset_' + filename + '.json', 'w') as f:
            json.dump(data, f)

        return STATIC_SUCCESS_TEMPLATE.substitute(homepage="")


# add controlEnabled bool array for init function
def init_c_lib():
    new_preset = (16 * 8 * AE_PRESET)()
    for bank_num in range(0, 16):
        for preset_num in range(0, 8):
            read_pedal = [0, 0, 0, 0, 0, 0, 0]
            read_enable = [False, False, False, False, False, False, False]
            control_enable = [False, False]
            new_preset[bank_num*8 + preset_num].preset = preset_num
            new_preset[bank_num*8 + preset_num].bank = bank_num

            filename = "%d_%d" % (bank_num + 1, preset_num + 1)
            if not os.path.isfile('data/preset_' + filename + '.json'):
                break

            with open('data/preset_' + filename + '.json', 'r') as f:
                data = json.load(f)

            for i in range(1, 8):
                read_pedal[i - 1] = data['pedal_pos' + str(i)]
                read_enable[i - 1] = data['enabled_pos' + str(i)]

            control_enable[1] = data['controlEnabled1']
            control_enable[0] = data['controlEnabled2']

            new_preset[bank_num*8 + preset_num].pedalOrder = (c_int * 7)(*read_pedal)
            new_preset[bank_num*8 + preset_num].enabled = (c_bool * 7)(*read_enable)
            new_preset[bank_num*8 + preset_num].controlEnabled = (c_bool*2)(*control_enable)
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
        remote.updateInfo(deepcopy(remoteInfo))
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, deepcopy(lcdInfo))
    return 0


def set_preset_py(preset):
    global lcd, remote, lcdInfo, remoteInfo
    lcdInfo["preset"] = c_int(preset).value
    remoteInfo["preset"] = c_int(preset).value

    bank = c_int(0)
    c_lib.get_bank(byref(bank))

    # read from file for preset name
    filename = "%d_%d" % (bank.value, c_int(preset).value)
    presetName = ''
    bankName = ''
    if os.path.isfile('data/preset_' + filename + '.json'):
        with open('data/preset_' + filename + '.json', 'r') as f:
            data = json.load(f)
            presetName = data['preset_name']
            bankName = data['bank_name']

    lcdInfo["presetName"] = presetName
    lcdInfo["bankName"] = bankName

    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, deepcopy(lcdInfo))
    return 0


def set_mode_py(mode):
    global lcd, remote, lcdInfo, remoteInfo
    lcdInfo["pedalMode"] = c_bool(mode).value
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, deepcopy(lcdInfo))
    return 0


def set_bypass_py(bypass):
    global lcd, remote, lcdInfo, remoteInfo
    lcdInfo["bypassMode"] = c_bool(bypass).value
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, deepcopy(lcdInfo))
    return 0


def set_mute_py(mute):
    global lcd, remote, lcdInfo, remoteInfo
    lcdInfo["muteMode"] = c_bool(mute).value
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, deepcopy(lcdInfo))
    return 0


def set_pedals_py(pedals, enabled, presence, control):
    global lcd, lcdInfo
    lcdInfo["pedals"] = [pedals[i] for i in range(0, 7)]
    lcdInfo["enabled"] = [enabled[i] for i in range(0, 7)]
    lcdInfo["presence"] = [presence[i] for i in range(0, 9)]
    lcdInfo["control"] = [control[i] for i in range(0, 2)]
    if lcd is not None:
        guiInvoker.invoke(lcd.updateInfo, deepcopy(lcdInfo))
    return 0

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    addr = ''
    try:
        ip = socket.inet_ntoa(fcntl.ioctl(
            s.fileno(),
            0x8915,  # SIOCGIFADDR
            struct.pack('256s', bytes(ifname[:15], 'utf-8'))
        )[20:24])
        addr = ip + ":5052"
    except:
        pass
    finally:
        return addr


def update_ip_address():
    global lcdInfo
    while True:
        ip = get_ip_address("wlan0")
        if ip != lcdInfo["webAddress"]:
            print("Web Address Changed: '" + ip + "'")
            lcdInfo["webAddress"] = ip
            if lcd is not None:
                guiInvoker.invoke(lcd.updateInfo, deepcopy(lcdInfo))
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
    lcdInfo["preset"] = remoteInfo["preset"] = preset.value
    pedals = (c_int * 7)()
    enabled = (c_bool * 7)()
    presence = (c_bool * 9)()
    control = (c_bool * 2)()
    c_lib.get_pedals(pedals, enabled, presence, control)
    lcdInfo["pedals"] = [pedals[i] for i in range(0, 7)]
    lcdInfo["enabled"] = [enabled[i] for i in range(0, 7)]
    lcdInfo["presence"] = [presence[i] for i in range(0, 9)]
    lcdInfo["control"] = [control[i] for i in range(0, 2)]

    # read from file for preset name
    filename = "%d_%d" % (bank.value, preset.value)
    presetName = ''
    bankName = ''
    if os.path.isfile('data/preset_' + filename + '.json'):
        with open('data/preset_' + filename + '.json', 'r') as f:
            data = json.load(f)
            presetName = data['preset_name']
            bankName = data['bank_name']

    # TODO in the future, get this from a file
    lcdInfo["hardwareVersion"] = "Rev B"
    lcdInfo["softwareVersion"] = "0.1 [Alpha]"
    lcdInfo["website"] = "https://design.ece.msstate.edu/2017/team_meadows/"

    lcdInfo["presetName"] = presetName
    lcdInfo["bankName"] = bankName
    lcdInfo["webAddress"] = get_ip_address("wlan0")
    lcdInfo["remoteID"] = None
    lcdInfo["pedalMode"] = mode.value
    lcdInfo["bypassMode"] = bypass.value
    lcdInfo["muteMode"] = mute.value
    lcdInfo["webEnabled"] = True
    lcdInfo["remotePaired"] = False
    remoteInfo["paired"] = None
    remoteInfo["id"] = ''

    print("Web Address: '" + lcdInfo["webAddress"] + "'")

    threading.Thread(target=update_ip_address).start()


def init_control():
    global set_preset_py_func, set_bank_py_func, set_mode_py_func, set_bypass_py_func, set_mute_py_func, set_pedals_py_func
    global set_preset_py_func_type, set_bank_py_func_type, set_mode_py_func_type, set_bypass_py_func_type, set_mute_py_func_type, set_pedals_func_type

    set_preset_py_func_type = CFUNCTYPE(c_int, c_int)
    set_preset_py_func = set_preset_py_func_type(set_preset_py)

    set_bank_py_func_type = CFUNCTYPE(c_int, c_int)
    set_bank_py_func = set_bank_py_func_type(set_bank_py)

    set_mode_py_func_type = CFUNCTYPE(c_int, c_bool)
    set_mode_py_func = set_mode_py_func_type(set_mode_py)

    set_bypass_py_func_type = CFUNCTYPE(c_int, c_bool)
    set_bypass_py_func = set_bypass_py_func_type(set_bypass_py)

    set_mute_py_func_type = CFUNCTYPE(c_int, c_bool)
    set_mute_py_func = set_mute_py_func_type(set_mute_py)

    set_pedals_py_func_type = CFUNCTYPE(c_int, POINTER(c_int), POINTER(c_bool), POINTER(c_bool), POINTER(c_bool))
    set_pedals_py_func = set_pedals_py_func_type(set_pedals_py)

    c_lib.register_callbacks(set_preset_py_func,
                             set_bank_py_func,
                             set_mode_py_func,
                             set_bypass_py_func,
                             set_mute_py_func,
                             set_pedals_py_func)


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
        remote.updateInfo(deepcopy(remoteInfo))
    lcdInfo = info.copy()


def remoteUpdate(info):
    global lcd, remote, lcdInfo, remoteInfo
    if info["bank"] != remoteInfo["bank"]:
        # read from file for bank name
        filename = "%d_%d" % (info["bank"], info["preset"])
        bankName = ''
        if os.path.isfile('data/preset_' + filename + '.json'):
            with open('data/preset_' + filename + '.json', 'r') as f:
                data = json.load(f)
                bankName = data['preset_name']

        lcdInfo["bankName"] = bankName
        lcdInfo["bank"] = info["bank"]
        set_bank_c(info["bank"])
    if info["preset"] != remoteInfo["preset"]:
        # read from file for preset name
        filename = "%d_%d" % (info["bank"], info["preset"])
        presetName = ''
        if os.path.isfile('data/preset_' + filename + '.json'):
            with open('data/preset_' + filename + '.json', 'r') as f:
                data = json.load(f)
                presetName = data['preset_name']

        lcdInfo["presetName"] = presetName
        lcdInfo["preset"] = info["preset"]
        set_preset_c(info["preset"])
    if info["id"] != remoteInfo["id"]:
        lcdInfo["remoteID"] = info["id"]
    if info["paired"] != remoteInfo["paired"]:
        lcdInfo["remotePaired"] = info["paired"]
    guiInvoker.invoke(lcd.updateInfo, deepcopy(lcdInfo))
    remoteInfo = info.copy()


def run_gui():
    global lcd, guiInvoker
    app = QApplication(sys.argv)
    #signal.signal(signal.SIGINT, signal.SIG_DFL)
    lcd = LCDWindow(deepcopy(lcdInfo), lcdUpdate)
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
    remote = Remote(deepcopy(remoteInfo), remoteUpdate)
    threading.Thread(target=run_app).start()
    run_gui()
