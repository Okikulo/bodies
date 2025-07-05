# bodies

![](resources/big_bodies.gif)

# Dependencies 
- SFML 2.6.1
```bash
# Arch Linux
sudo pacman -S sfml
# or if already have sfml installed, use downgrade and choose version 2.6.1
yay -S downgrade
sudo downgrade sfml

# Ubuntu/Debian
sudo apt install libsfml-dev

# Fedora
sudo dnf install SFML-devel
```
# Usage
```bash
./bin/nbody_simulation_serial [numBodies] [dt] [softening]
```
```bash
./bin/nbody_simulation_omp [numBodies] [dt] [softening]
```
