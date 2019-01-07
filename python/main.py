#!/usr/bin/python3

import subprocess
import time
import os
import sys

from subprocess import PIPE

import pymail

blacklist = [
"brock800_1.clq",
"brock800_2.clq",
"brock800_3.clq",
"brock800_4.clq",
"keller6.clq",
"DSJC1000_5.clq",
"p_hat700-1.clq",
"p_hat700-2.clq",
"p_hat700-3.clq",
"p_hat1000-1.clq",
"p_hat1000-2.clq",
"p_hat1000-3.clq",
"p_hat1500-1.clq",
"p_hat1500-2.clq",
"p_hat1500-3.clq",
"san1000.clq",
"C1000.9.clq",
"C2000.5.clq",
"C2000.9.clq",
"C4000.5.clq"
]

whitelist = [
"karate.dimacs"
]

class TimeoutLowError(Exception):
    pass

def run_timeout(args, time_low, time_high):
    #FIXME measure time here?
    print(args, file=sys.stderr)
    proc = subprocess.Popen(args, stdout=PIPE, stderr=PIPE)
    try:
        time.sleep(time_low)
        if proc.poll() != None:
            return 1, "", ""
        outs, errs = proc.communicate(timeout=time_high)
        return 0, outs, errs
    except subprocess.TimeoutExpired:
        proc.kill()
        # outs, errs = proc.communicate()
        return 2, "", ""
    except Exception as e:
        return 3, str(e), ""

def str_code(code, outs=""):
    msg = {
        0: "OK",
        1: "Time is too low",
        2: "Time is too high",
        3: "Error: " + str(outs)
    }
    return msg[code]

# UGLY
def parse_RDS_output(out):
    out = str(out)
    # parse time
    txt_time = "was completed in "
    i = out.rfind(txt_time)
    j = out.find(" ", i+len(txt_time))
    return_time = out[i+len(txt_time) : j]

    txt_sol = "solution size: "
    i = out.rfind(txt_sol)
    j = out.find("\\n", i)
    return_sol = out[i+len(txt_sol) : j]

    return return_time, return_sol

TIMELIM = 300 # 5 minutes

def work(directory, problem):
    output = []
    orders = [ "", "-vd", "-vdg", "-vd2", "-vclq" ]
    for i in orders.copy():
        orders.append(i + " -vrev")
    # walk through a dir for graphs
    for dirName, subdirs, files in os.walk(directory):
        for fname in files:
            if not fname.startswith(".") and not fname in blacklist: # and fname == "johnson8-2-4.clq":
                realfname = os.path.join(dirName, fname)
                best_time = TIMELIM
                try:
                    for order in orders:
                        args = [ "../RDS" ] + order.split() + problem.split() + [realfname]
                        code, outs, errs = run_timeout(args, 0, best_time + 2.0)
                        if code == 0:
                            try:
#                                print(outs)
                                timeres, _ = parse_RDS_output(outs)
                                timeres = float(timeres)
                                output.append("{} & {} & {} & {}".format(fname, problem, order, timeres))
                                if timeres < best_time:
                                    best_time = timeres
                            except:
#                                print(str(outs))
#                                print(str(errs))
#                                print(args)
                                raise
                        elif code == 2:
                            output.append("{} & {} & {} & TLE/{}".format(fname, problem, order, best_time))

                except Exception as e:
#                    print(str(e))
                    best_order = "Error"
                    best_time = 10000.
    return "\n".join(output)

def main():
    email = []
    the_directory = "/home/lykhovyd/GRAPH_DATA/rds_test/"
#    the_directory = "/home/tsumiman/Documents/Graphs/DIMACS/"
    problems = [ "-c", "-s", "-iuc", "-fn", "-bn", "-chn", "-bc" ] #, "-sd 1", "-sd 2", "-sd 3", "-sp 2", "-sp 3", "-sp 4" ]
    for i in problems.copy():
        problems.append("-comp " + i)

    for problem in problems:
        msg = work(the_directory, problem)
        email.append("Results for problem %s\n\n" % problem)
        email.append(msg)
        email.append("\n--------------------\n\n")
        data = "".join(email)
        pymail.SendMail("RDS Results for %s" % problem, data)
        email = []

main()

#debug_cmd = ["../RDS", "-iuc", "/home/lykhovyd/GRAPH_DATA/DIMACS_all_ascii/brock200_2.clq"]
#code, outs, errs = run_timeout(debug_cmd, 1, 10)
#print(str_code(code, outs))
#print(parse_RDS_output(outs))
