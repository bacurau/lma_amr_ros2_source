import pandas as pd
import plotly.express as px

bag_name = "robot_not_moving_sqlite"
csv_file_name="imu_angular_velocity.csv"
csv_file_path = f"../../bags/{bag_name}/{csv_file_name}"

df = pd.read_csv(csv_file_path)


fig = px.line(
    df,
    x="time",
    y=["vector.x", "vector.y", "vector.z"],
    labels={
        "time": "Time [s]",
        "value": "Value",
        "variable": "Axis"
    },
    title="X, Y, Z over time"
)

fig.show()