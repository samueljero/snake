#Testing System Configuration
#Samuel Jero <sjero@purdue.edu>
import os

system_home = os.path.split(os.path.dirname(os.path.realpath(__file__)))[0]

#Basic Config
protocol = "TCP"

#Coordinator
coord_port = 9991

#Testing Config
server_ip = "10.1.2.3"
server_port = 80
client_ip = "10.1.2.2"
client_port = 5555
default_window = 20000

#Logs
logdir = os.path.abspath(os.path.join(system_home,protocol))
coord_log = logdir + "/coordinator.log"
results_log = logdir + "/results.log"
checkpoint = logdir + "/checkpoint.ck"
