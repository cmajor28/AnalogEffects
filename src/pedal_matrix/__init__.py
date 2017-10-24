from flask import Flask, jsonify, request, render_template
from flask_bootstrap import Bootstrap
import json
import os.path
from ctypes import *
from bluetooth.remote import *
from gui.lcd import *
import threading

app = Flask(__name__)
Bootstrap(app)

c_lib = cdll.LoadLibrary("./aeffects.so")  # call construct at startup, def __init (in essence, calling C constructor)

class AE_PRESET(Structure):
    _fields_ = [("bank", c_int),
                ("preset", c_int),
                ("pedalOrder", c_int * 7),
                ("enabled", c_bool * 7),
                ("controlEnabled", c_bool * 2)]

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

        #c_lib.aeffects_update(byref(new_preset))

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
    return 0


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
    info = {}
    info["bank"] = int(bank)
    Remote.updateInfo(info)
    LCD.updateInfo(info)
    return


def set_preset_py(preset):
    info = {}
    info["preset"] = int(preset)
    Remote.updateInfo(info)
    LCD.updateInfo(info)
    return


def set_mode_py(mode):
    info = {}
    info["mode"] = bool(mode)
    LCD.updateInfo(info)
    return


def set_bypass_py(bypass):
    info = {}
    info["bypass"] = bool(bypass)
    LCD.updateInfo(info)
    return


def set_mute_py(mute):
    info = {}
    info["mute"] = bool(mute)
    LCD.updateInfo(info)
    return

if __name__ == "__main__":
    #init_c_lib()
    global Remote   # need to specify object
    global LCD      # need to specify object
    app.run(host='0.0.0.0', port=5052, debug=True)
