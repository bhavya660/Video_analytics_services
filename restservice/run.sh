#sudo docker run -v /dev/shm:/dev/shm -p 8080:80 rest_service
sudo docker run -it --rm -p 8080:80 --name rest_service -v /dev/shm:/dev/shm rest_service
