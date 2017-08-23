from flask import Flask, jsonify, request

app = Flask(__name__)

#@app.route('/')
@app.route('/PresetsSaved', methods=['POST'])
def index():
    pedal = [0, 0, 0, 0, 0, 0, 0]
    pedal_in = [0, 0, 0, 0, 0, 0, 0]
    pedal_out = [0, 0, 0, 0, 0, 0, 0]
    
    pedal[0] = request.json['pedal_pos1']
    pedal[1] = request.json['pedal_pos2']
    pedal[2] = request.json['pedal_pos3']
    pedal[3] = request.json['pedal_pos4']
    pedal[4] = request.json['pedal_pos5']
    pedal[5] = request.json['pedal_pos6']
    pedal[6] = request.json['pedal_pos7']
    #print(pedal)
    # 0 indicates unused pedal, 8 indicates final output
    pedal_in[0] = pedal[0]
    pedal_out[0] = pedal[1]
    pedal_in[1] = pedal[1]
    pedal_out[1] = pedal[2]
    pedal_in[2] = pedal[2]
    pedal_out[2] = pedal[3]
    pedal_in[3] = pedal[3]
    pedal_out[3] = pedal[4]
    pedal_in[4] = pedal[4]
    pedal_out[4] = pedal[5]
    pedal_in[5] = pedal[5]
    pedal_out[5] = pedal[6]
    pedal_in[6] = pedal[6]
    if pedal[6] == 0:
        pedal_out[6] = 0
    else:
        pedal_out[6] = 8

    for i in range(len(pedal)):
        if pedal[i] == 0:
            pedal_out[(i - 1)] = 8
            break

    #print(pedal_out)
    #return "Dr Dabs is gonna love this shit"
    return jsonify({
    'pedal_pos1_in': pedal_in[0],
    'pedal_pos1_out': pedal_out[0],
    'pedal_pos2_in': pedal_in[1],
    'pedal_pos2_out': pedal_out[1],
    'pedal_pos3_in': pedal_in[2],
    'pedal_pos3_out': pedal_out[2],
    'pedal_pos4_in': pedal_in[3],
    'pedal_pos4_out': pedal_out[3],
    'pedal_pos5_in': pedal_in[4],
    'pedal_pos5_out': pedal_out[4],
    'pedal_pos6_in': pedal_in[5],
    'pedal_pos6_out': pedal_out[5],
    'pedal_pos7_in': pedal_in[6],
    'pedal_pos7_out': pedal_out[6],
    })

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5052, debug=True)
