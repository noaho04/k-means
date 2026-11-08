game:
		g++ main.cpp -std=c++23 \
				-I raylib-6.2_linux_amd64/include \
				   raylib-6.2_linux_amd64/lib/libraylib.a \
				-lGL -lm -lpthread -ldl -lrt -lX11 -o app
