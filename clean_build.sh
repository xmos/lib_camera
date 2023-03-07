sudo rm -r build/*
sudo rm -r modules/core modules/i2c modules/mipi 
sleep 2
echo "Running wes update"
west update