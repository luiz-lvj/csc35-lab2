g++ -o client client_drone.cpp drone.cpp messages.cpp

g++ -o server server.cpp drone.cpp

g++ -o mission client_mission.cpp