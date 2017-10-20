from flask import Flask, jsonify, request, render_template
from flask_bootstrap import Bootstrap
import json
import os.path
from ctypes import *

app = Flask(__name__)
Bootstrap(app)

c_lib = cdll.LoadLibrary("./aeffects.so")  # call construct at startup, def __init (in essence, calling C constructor)

class AE_PRESET(Structure):
    _fields_ = [("bank", c_int),
                ("preset", c_int),
                ("pedalOrder", c_int * 7),
                ("enabled", c_bool * 7),
                ("controlEnabled", c_bool * 2)]

@app.route('/')
def index():
    return render_template('index.html')

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


@app.route('/Presets', methods=['POST'])
def preset_order():
    pedal = [0, 0, 0, 0, 0, 0, 0]
    enable = [0, 0, 0, 0, 0, 0, 0]
    controlEnabled = [0, 0]

    controlEnabled[0] = request.json['controlEnabled1']
    controlEnabled[1] = request.json['controlEnabled2']

    bank_name = request.json['bank_name']
    bank_num = request.json['bank_num']
    preset_name = request.json['preset_name']
    preset_num = request.json['preset_num']

    pedal_pos1_name = request.json['pedal_pos1_name']
    pedal_pos2_name = request.json['pedal_pos2_name']
    pedal_pos3_name = request.json['pedal_pos3_name']
    pedal_pos4_name = request.json['pedal_pos4_name']
    pedal_pos5_name = request.json['pedal_pos5_name']
    pedal_pos6_name = request.json['pedal_pos6_name']
    pedal_pos7_name = request.json['pedal_pos7_name']

    pedal[0] = request.json['pedal_pos1']
    pedal[1] = request.json['pedal_pos2']
    pedal[2] = request.json['pedal_pos3']
    pedal[3] = request.json['pedal_pos4']
    pedal[4] = request.json['pedal_pos5']
    pedal[5] = request.json['pedal_pos6']
    pedal[6] = request.json['pedal_pos7']

    enable[0] = request.json['enabled_pos1']
    enable[1] = request.json['enabled_pos2']
    enable[2] = request.json['enabled_pos3']
    enable[3] = request.json['enabled_pos4']
    enable[4] = request.json['enabled_pos5']
    enable[5] = request.json['enabled_pos6']
    enable[6] = request.json['enabled_pos7']

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
        "pedal_pos1_name": pedal_pos1_name,
        "pedal_pos2_name": pedal_pos2_name,
        "pedal_pos3_name": pedal_pos3_name,
        "pedal_pos4_name": pedal_pos4_name,
        "pedal_pos5_name": pedal_pos5_name,
        "pedal_pos6_name": pedal_pos6_name,
        "pedal_pos7_name": pedal_pos7_name,
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


if __name__ == "__main__":
    init_c_lib()
    app.run(host='0.0.0.0', port=5052, debug=True)
