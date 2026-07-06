from subprocess import Popen, TimeoutExpired, check_output, PIPE, run
from time import sleep
import os

bag_name = "robot_not_moving"
csv_file_path = f"./csv_files/{bag_name}.csv"
topic = "/imu/angular_velocity"

run_bag_command = ["ros2", "bag", "play", f"../../bags/{bag_name}", "-r", "20", "-p"]
run_bag_process = Popen(run_bag_command)


with open(csv_file_path, "w") as csv_file:

  get_topic_type_command = ["ros2", "topic", "info", topic]
  get_topic_type = check_output(get_topic_type_command, text=True)
  topic_type = get_topic_type.split()[1]
  
  get_topic_description_command = ["ros2", "interface", "show", topic_type]
  get_topic_description = check_output(get_topic_description_command, text=True)
  previous_leading_space = 0
  previous_line=""
  header_list=[]
  # Source - https://stackoverflow.com/a/1140966
  # Posted by Redwood, modified by community. See post 'Timeline' for change history
  # Retrieved 2026-07-02, License - CC BY-SA 2.5
  lines = os.linesep.join([s for s in get_topic_description.splitlines() if s and s[0] != "#"])
  lines = lines.splitlines()
  number_of_lines=len(lines)
  for i,line in enumerate(lines):
    leading_space = len(line) - len(line.lstrip())
    if i > 0 and leading_space <= previous_leading_space:
      header_list.append(previous_line.split()[1])
    previous_line=line
    previous_leading_space=leading_space

  header_list.append(lines[number_of_lines-1].split()[1])
  header_string=','.join(header_list)
  csv_file.write(header_string)
  csv_file.write('\n')
  csv_file.flush()
  save_csv_command = ["ros2", "topic", "echo", topic, "--csv"]
  save_csv_process = Popen(save_csv_command, stdout=csv_file, text=True)

run_bag_process.wait()

save_csv_process.terminate()

save_csv_process.wait()




