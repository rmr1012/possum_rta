import sys

from epsolar_tracer.client import EPsolarTracerClient
from epsolar_tracer.enums.RegisterTypeEnum import RegisterTypeEnum

default_port = '/dev/cu.usbserial-FT2R9MBN'

port = input('Enter serial port to use [{}]:'.format(default_port)) or default_port

client = EPsolarTracerClient(port=port)

if client.connect():
    print('Connected successfully to {}'.format(port))
else:
    print('Connection failed to {}'.format(port))
    sys.exit(1)

response = client.read_device_info()
print("Manufacturer: {}".format(repr(response.information[0])))
print("Model: {}".format(repr(response.information[1])))
print("Version: {}".format(repr(response.information[2])))

response = client.read_input(RegisterTypeEnum.BATTERY_SOC)
print(str(response))
response = client.read_input(RegisterTypeEnum.CHARGING_EQUIPMENT_OUTPUT_CURRENT)
print(str(response))


# config batt:
# response = client.write_output(RegisterTypeEnum.BATTERY_TYPE, 0x0000)

response = client.read_input(RegisterTypeEnum.BATTERY_TYPE)
print(str(response))

# response = client.write_output(RegisterTypeEnum.BATTERY_CAPACITY, 60)
response = client.read_input(RegisterTypeEnum.BATTERY_CAPACITY)
print(str(response))

# response = client.write_output(RegisterTypeEnum.HIGH_VOLT_DISCONNECT, 17.2)
response = client.read_input(RegisterTypeEnum.HIGH_VOLT_DISCONNECT)
print(str(response))

# response = client.write_output(RegisterTypeEnum.CHARGING_LIMIT_VOLTAGE, 16.8)
response = client.read_input(RegisterTypeEnum.CHARGING_LIMIT_VOLTAGE)
print(str(response))

# response = client.write_output(RegisterTypeEnum.OVER_VOLTAGE_RECONNECT, 16.93)
response = client.read_input(RegisterTypeEnum.OVER_VOLTAGE_RECONNECT)
print(str(response))

# response = client.write_output(RegisterTypeEnum.EQUALIZATION_VOLTAGE, 16.9)
response = client.read_input(RegisterTypeEnum.EQUALIZATION_VOLTAGE)
print(str(response))

# response = client.write_output(RegisterTypeEnum.BOOST_VOLTAGE, 16.67)
response = client.read_input(RegisterTypeEnum.BOOST_VOLTAGE)
print(str(response))

# response = client.write_output(RegisterTypeEnum.FLOAT_VOLTAGE, 16.26)
response = client.read_input(RegisterTypeEnum.FLOAT_VOLTAGE)
print(str(response))

# response = client.write_output(RegisterTypeEnum.BOOST_RECONNECT_VOLTAGE, 16.13)
response = client.read_input(RegisterTypeEnum.BOOST_RECONNECT_VOLTAGE)
print(str(response))

# response = client.write_output(RegisterTypeEnum.LOW_VOLTAGE_RECONNECT, 14.23)
response = client.read_input(RegisterTypeEnum.LOW_VOLTAGE_RECONNECT)
print(str(response))

# response = client.write_output(RegisterTypeEnum.UNDER_VOLTAGE_RECOVER, 14.91)
response = client.read_input(RegisterTypeEnum.UNDER_VOLTAGE_RECOVER)
print(str(response))

# response = client.write_output(RegisterTypeEnum.UNDER_VOLTAGE_WARNING, 14.23)
response = client.read_input(RegisterTypeEnum.UNDER_VOLTAGE_WARNING)
print(str(response))

# response = client.write_output(RegisterTypeEnum.LOW_VOLTAGE_DISCONNECT, 12.6)
response = client.read_input(RegisterTypeEnum.LOW_VOLTAGE_DISCONNECT)
print(str(response))

# response = client.write_output(RegisterTypeEnum.DISCHARGING_LIMIT_VOLTAGE, 12.6)
response = client.read_input(RegisterTypeEnum.DISCHARGING_LIMIT_VOLTAGE)
print(str(response))
# response = client.write_output("Manual control the load", 0)

# print(str(response))

"""
for reg_type, reg in registers.items():
    # print
    # print reg
    value = client.read_input(reg.name)
    print(value)
    # if value.value is not None:
    #    print client.write_output(reg.name,value.value)
for reg_type, reg in coils.items():
    # print
    # print reg
    value = client.read_input(reg.name)
    print(value)
    # print client.write_output(reg.name,value.value)
"""
client.close()
