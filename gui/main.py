from flask import Flask, jsonify, request
from flask_cors import CORS
import os
from subprocess import Popen, PIPE

app = Flask(__name__)
"""
CORS is required to interact with vue.js
"""
CORS(app, resources={r"/*": {'origins': "*"}})

memory_list = []
core1_reg_states = []
core0_reg_states = []

core1_pipeline_states = []
core0_pipeline_states = []

core1_stats = []
core0_stats = []


def clear_mem():
    global memory_list
    memory_list = [{str(i): 0 for i in range(1024)}]


def clear_cores_reg():
    global core0_reg_states, core1_reg_states
    core0_reg_states = [
        {
            "0": "0",
            "1": "0",
            "10": "0",
            "11": "0",
            "12": "0",
            "13": "0",
            "14": "0",
            "15": "0",
            "16": "0",
            "17": "0",
            "18": "0",
            "19": "0",
            "2": "0",
            "20": "0",
            "21": "0",
            "22": "0",
            "23": "0",
            "24": "0",
            "25": "0",
            "26": "0",
            "27": "0",
            "28": "0",
            "29": "0",
            "3": "0",
            "30": "0",
            "31": "0",
            "4": "0",
            "5": "0",
            "6": "0",
            "7": "0",
            "8": "0",
            "9": "0"
        }
    ]
    core1_reg_states = [
        {
            "0": "0",
            "1": "0",
            "10": "0",
            "11": "0",
            "12": "0",
            "13": "0",
            "14": "0",
            "15": "0",
            "16": "0",
            "17": "0",
            "18": "0",
            "19": "0",
            "2": "0",
            "20": "0",
            "21": "0",
            "22": "0",
            "23": "0",
            "24": "0",
            "25": "0",
            "26": "0",
            "27": "0",
            "28": "0",
            "29": "0",
            "3": "0",
            "30": "0",
            "31": "0",
            "4": "0",
            "5": "0",
            "6": "0",
            "7": "0",
            "8": "0",
            "9": "0"
        }
    ]


@app.route('/mem')
def memory():
    global memory_list
    return jsonify(memory_list)


@app.route('/clear')
def clear():
    global core0_pipeline_states, core1_pipeline_states, core0_stats, core1_stats
    core1_pipeline_states = []
    core0_pipeline_states = []
    core0_stats = {}
    core1_stats = {}
    clear_cores_reg()
    return {"message": "done"}


@app.route('/run', methods=["POST"])
def run():
    global memory_list, core1_reg_states, core0_reg_states, core1_pipeline_states, core0_pipeline_states, core0_stats, core1_stats

    core1_reg_states = []
    core0_reg_states = []
    core1_pipeline_states = []
    core0_pipeline_states = []
    core0_stats = {}
    core1_stats = {}
    memory_list = []

    if request.method == "POST":
        try:
            """
            Remove the files of previous run
            """
            if not os.path.exists("a.out"):
                os.system("g++ main.cpp")
            if os.path.exists("data/core1_reg.txt"):
                os.remove("data/core1_reg.txt")
            if os.path.exists("data/core0_reg.txt"):
                os.remove("data/core0_reg.txt")
            if os.path.exists("data/core1_pipe.txt"):
                os.remove("data/core1_pipe.txt")
            if os.path.exists("data/core0_pipe.txt"):
                os.remove("data/core0_pipe.txt")

            instruction = []
            file0 = "codes/selection_sort.s"
            file1 = "codes/bubble_sort.s"
            if request.form["code0"]:
                content = request.form["code0"]
                content = content.replace("\r\n", "\n").replace(",", " ")
                with open("codes/slot0.s", "w") as slot0_file:
                    slot0_file.write(content)
                file0 = "codes/slot0.s"

            if request.form["code1"]:
                content = request.form["code1"]
                content = content.replace("\r\n", "\n").replace(",", " ")
                with open("codes/slot1.s", "w") as slot0_file:
                    slot0_file.write(content)
                file1 = "codes/slot1.s"

            if request.form["pipeline"] == "true":
                if request.form["forward"] == "true":
                    instruction = ["./a.out", file0,
                                   file1, "true", "true"]
                else:
                    instruction = ["./a.out", file0,
                                   file1, "true", "false"]
            else:
                instruction = ["./a.out", file0,
                               file1, "false", "false"]

            process = Popen(instruction, shell=False, stdout=PIPE, stdin=PIPE)
            input = f"{request.form['addi']} {request.form['add']} {request.form['div']} {request.form['mul']} {request.form['sub']}"

            stdout, stderr = process.communicate(input=str.encode(input))
            memory_dict = {}
            with open("data/memory_before.txt") as mem_file:
                memory = mem_file.read()
                memory.replace("\r\n", "\n")
                memory = memory.split("\n")
                memory_dict = {}
                for i in memory[:-1]:
                    memory_dict[int(i.split(",")[0])] = int(i.split(",")[1])
                memory_list.append(memory_dict)
            with open("data/memory_after.txt") as mem_file:
                memory = mem_file.read()
                memory.replace("\r\n", "\n")
                memory = memory.split("\n")
                memory_dict = {}
                for i in memory[:-1]:
                    memory_dict[int(i.split(",")[0])] = int(i.split(",")[1])
                memory_list.append(memory_dict)
            with open("data/core1_reg.txt") as core_reg_file:
                reg_states = core_reg_file.read()
                reg_states.replace("\r\n", "\n")
                reg_states = reg_states.split("\n")
                if reg_states[-1] == "":
                    reg_states = reg_states[:-1]
                for state in reg_states:
                    reg_dict = {}
                    content = state.split("\t")
                    for data in content:
                        if data != "":
                            reg, value = data.split(" ")
                            reg_dict[reg[1:]] = value

                    core1_reg_states.append(reg_dict)
            with open("data/core0_reg.txt") as core_reg_file:
                reg_states = core_reg_file.read()
                reg_states.replace("\r\n", "\n")
                reg_states = reg_states.split("\n")
                if reg_states[-1] == "":
                    reg_states = reg_states[:-1]
                for state in reg_states:
                    reg_dict = {}
                    content = state.split("\t")
                    for data in content:
                        if data != "":
                            reg, value = data.split(" ")
                            reg_dict[reg[1:]] = value

                    core0_reg_states.append(reg_dict)

            if request.form["pipeline"] == "true":
                with open("data/core1_pipe.txt") as core_pipe_file:
                    states = core_pipe_file.read().replace("\r\n", "\n").split("\n")
                    if states[-1] == "":
                        states = states[:-1]
                    for state in states:
                        state_list = state.split(" ")
                        state_dict = {}
                        if (len(state_list) > 5):
                            state_list = state_list[:-1]
                        for _ in range(len(state_list)):
                            if state_list[_] == "-1":
                                state_dict[f"{_}"] = "stall"
                            else:
                                state_dict[f"{_}"] = str(
                                    int(state_list[_])-856)
                        core1_pipeline_states.append(state_dict)

                with open("data/core0_pipe.txt") as core_pipe_file:
                    states = core_pipe_file.read().replace("\r\n", "\n").split("\n")
                    if states[-1] == "":
                        states = states[:-1]
                    for state in states:
                        state_list = state.split(" ")
                        state_dict = {}
                        if (len(state_list) > 5):
                            state_list = state_list[:-1]
                        for _ in range(len(state_list)):
                            if state_list[_] == "-1":
                                state_dict[f"{_}"] = "stall"
                            else:
                                state_dict[f"{_}"] = state_list[_]
                        core0_pipeline_states.append(state_dict)

                with open("data/stats.txt") as stats_file:
                    stats = stats_file.read().replace("\r\n", "\n").split("\n")
                    if stats[-1] == "":
                        stats = stats[:-1]
                    core0_stats = {"IC": stats[0], "HC": stats[2], "Clock": stats[4],
                                   "IPC": round(int(stats[0])/int(stats[4]), 3)}
                    core1_stats = {"IC": stats[1], "HC": stats[3], "Clock": stats[5],
                                   "IPC": round(int(stats[1])/int(stats[5]), 3)}
        except:
            clear_cores_reg()
    return {"message": "Done"}


@app.route('/load', methods=["POST"])
def load():
    return None


@app.route('/core/0/reg')
def core0_reg():
    return jsonify(core0_reg_states)


@app.route('/core/1/reg')
def core1_reg():
    return jsonify(core1_reg_states)


@app.route('/core/0/pipe')
def core0_pipe():
    return jsonify(core0_pipeline_states)


@app.route('/core/1/pipe')
def core1_pipe():
    return jsonify(core1_pipeline_states)


@app.route('/core/0/stats')
def core0_stats_fun():
    return jsonify(core0_stats)


@app.route('/core/1/stats')
def core1_stats_fun():
    return jsonify(core1_stats)


if __name__ == "__main__":
    clear_cores_reg()
    clear_mem()
    app.run(debug=True)
