./rm_exited.sh
#sudo docker run -it --name streaming_service streaming_service /bin/bash
# Volume mount for file
#sudo docker run -it --name streaming_service -v ./files:/files streaming_service /bin/bash

# Volume mount for shmsink and shmsrc pipeline
#sudo docker run -it --name streaming_service streaming_service -v /dev/shm:/dev/shm /bin/bash
# sudo docker run -it --rm --name streaming_service -v /dev/shm:/dev/shm streaming_service /bin/bash
sudo docker run -it --rm --network host --name video_service -v /dev/shm:/dev/shm video_service /bin/bash
