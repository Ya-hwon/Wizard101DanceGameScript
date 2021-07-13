// WizardDanceScript.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _USE_MATH_DEFINES

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <math.h>
#include <thread>
#include <vector>


using namespace std::chrono_literals;

#define THRESHHOLD 120
#define PRECISION 128


enum DIR {
	LEFT = 0x25,
	UP,
	RIGHT,
	DOWN,
	INV
};

const char* const DIRSTR[5] = { "LEFT", "UP", "RIGHT", "DOWN", "INV" };

class IOHandler {
public:

	IOHandler() {
		directions.push_back( INV );
		fileStream.open( "settings.txt", std::fstream::in );
		if ( !fileStream.good() ) {
			saveSettings();
		}
		else {
			std::string data;
			std::getline( fileStream, data );
			size = abs( std::stoi( data ) );
			std::getline( fileStream, data );
			baseX = abs( std::stoi( data ) );
			std::getline( fileStream, data );
			baseY = abs( std::stoi( data ) );
			fileStream.close();
		}
	}

	~IOHandler() {
		delete[] coords;
	}

	void saveSettings() {
		fileStream.open( "settings.txt", std::fstream::out | std::fstream::trunc );
		if ( !fileStream.good() ) { std::cerr << "\u001b[31mERROR: Could not create settings file.\u001b[37m\n"; std::this_thread::sleep_for( 3s ); std::exit( EXIT_FAILURE ); };
		fileStream << std::to_string( size ) << '\n' << std::to_string( baseX ) << '\n' << std::to_string( baseY ) << '\n';
		fileStream.close();
	}

	std::fstream fileStream;

	bool targetLockedIn = true;

	std::vector<DIR> directions;

	unsigned int round = 0;
	unsigned int current = 0;

	unsigned int size = 48;

	unsigned int baseX = 1279;
	unsigned int baseY = 1314;

	unsigned int* coords = nullptr;

	void generate() {
		coords = new unsigned int[PRECISION * 2];
		double step = ( 2 * M_PI ) / PRECISION;

		for ( unsigned int i = 0; i < PRECISION; i++ ) {
			coords[i * 2] = size * sin( i * step );
			coords[i * 2 + 1] = size * cos( i * step );
		}
	}

	void draw() {

		if ( targetLockedIn )return;

		HDC hdc = GetDC( NULL );

		if ( hdc == NULL ) {
					std::cerr << "\u001b[31mERROR: Could not get device context, retrying...\u001b[37m\n";
					hdc = GetDC( NULL );
					if ( hdc == NULL ) {
						std::cerr << "\u001b[31mERROR: Failed, exiting.\u001b[37m\n";
						std::this_thread::sleep_for( 3s );
						std::exit( EXIT_FAILURE );
					}
					std::cout << "\u001b[32mWorked.\u001b[37m\n";
				}

		POINT p;
		GetCursorPos( &p );
		baseX = p.x;
		baseY = p.y;

		//UP
		SetPixel( hdc, baseX, baseY - size / 1.2, RGB( 0, 255, 0 ) );
		SetPixel( hdc, baseX + size / 2, baseY - size / 10, RGB( 0, 255, 0 ) );
		SetPixel( hdc, baseX - size / 2, baseY - size / 10, RGB( 0, 255, 0 ) );

		//LEFT
		SetPixel( hdc, baseX - size / 1.2, baseY, RGB( 255, 0, 255 ) );
		SetPixel( hdc, baseX - size / 10, baseY + size / 2, RGB( 255, 0, 255 ) );
		SetPixel( hdc, baseX - size / 10, baseY - size / 2, RGB( 255, 0, 255 ) );

		//DOWN
		SetPixel( hdc, baseX, baseY + size / 1.2, RGB( 0, 0, 255 ) );
		SetPixel( hdc, baseX + size / 2, baseY + size / 10, RGB( 0, 0, 255 ) );
		SetPixel( hdc, baseX - size / 2, baseY + size / 10, RGB( 0, 0, 255 ) );

		//RIGHT
		SetPixel( hdc, baseX + size / 1.2, baseY, RGB( 255, 255, 0 ) );
		SetPixel( hdc, baseX + size / 10, baseY + size / 2, RGB( 255, 255, 0 ) );
		SetPixel( hdc, baseX + size / 10, baseY - size / 2, RGB( 255, 255, 0 ) );


		for ( unsigned int i = 0; i < PRECISION; i++ ) {
			SetPixel( hdc, baseX + coords[i * 2], baseY + coords[i * 2 + 1], RGB( 255, 0, 0 ) );
		}

		ReleaseDC( NULL, hdc );
	}

	void press( DIR dir ) {
		if ( dir == INV )return;
		std::cout << "Pressing " << DIRSTR[dir - LEFT] << ".\n";

		INPUT inputs[2] = {};
		ZeroMemory( inputs, sizeof( inputs ) );
		inputs[0].type = INPUT_KEYBOARD;
		inputs[0].ki.wVk = dir;
		inputs[1].type = INPUT_KEYBOARD;
		inputs[1].ki.wVk = dir;
		inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
		if ( SendInput( ARRAYSIZE( inputs ), inputs, sizeof( INPUT ) ) != ARRAYSIZE( inputs ) ) {
			std::cerr << "\u001b[31mERROR: Could not send input (keyboard).\u001b[37m\n";
		}
	}

	void reset() {
		directions.clear();
		round = 0;
		current = 0;
	}

	void read() {
		if ( !targetLockedIn )return;

		HDC hdc = GetDC( NULL );

		if ( hdc == NULL ) {
			std::cerr << "\u001b[31mERROR: Could not get device context, retrying...\u001b[37m\n";
			hdc = GetDC( NULL );
			if ( hdc == NULL ) {
				std::cerr << "\u001b[31mERROR: Failed, exiting.\u001b[37m\n";
				std::this_thread::sleep_for( 3s );
				std::exit( EXIT_FAILURE );
			}
			std::cout << "\u001b[32mWorked.\u001b[37m\n";
		}

		COLORREF CENTERPIXEL = GetPixel( hdc, baseX, baseY );

		if ( CENTERPIXEL == CLR_INVALID ) { std::cerr << "\u001b[31mERROR: Invalid pixel colour.\u001b[37m\n"; ReleaseDC( NULL, hdc ); return; }

		bool isYellowIsh = (int)GetRValue( CENTERPIXEL ) > 180 && (int)GetGValue( CENTERPIXEL ) > 180 && (int)GetBValue( CENTERPIXEL ) < 100;
		if ( !isYellowIsh ) { if ( directions.back() != INV )directions.push_back( INV ); ReleaseDC( NULL, hdc ); return; }

		COLORREF UPPIXELS[3];
		UPPIXELS[0] = GetPixel( hdc, baseX, baseY - size / 1.2 );
		UPPIXELS[1] = GetPixel( hdc, baseX + size / 2, baseY - size / 10 );
		UPPIXELS[2] = GetPixel( hdc, baseX - size / 2, baseY - size / 10 );
		if ( UPPIXELS[0] == CLR_INVALID || UPPIXELS[1] == CLR_INVALID || UPPIXELS[2] == CLR_INVALID ) { std::cerr << "\u001b[31mERROR: Invalid pixel colour.\u001b[37m\n"; ReleaseDC( NULL, hdc ); return; }

		COLORREF LEFTPIXELS[3];
		LEFTPIXELS[0] = GetPixel( hdc, baseX - size / 1.2, baseY );
		LEFTPIXELS[1] = GetPixel( hdc, baseX - size / 10, baseY + size / 2 );
		LEFTPIXELS[2] = GetPixel( hdc, baseX - size / 10, baseY - size / 2 );
		if ( LEFTPIXELS[0] == CLR_INVALID || LEFTPIXELS[1] == CLR_INVALID || LEFTPIXELS[2] == CLR_INVALID ) { std::cerr << "\u001b[31mERROR: Invalid pixel colour.\u001b[37m\n"; ReleaseDC( NULL, hdc ); return; }

		COLORREF DOWNPIXELS[3];
		DOWNPIXELS[0] = GetPixel( hdc, baseX, baseY + size / 1.2 );
		DOWNPIXELS[1] = GetPixel( hdc, baseX + size / 2, baseY + size / 10 );
		DOWNPIXELS[2] = GetPixel( hdc, baseX - size / 2, baseY + size / 10 );
		if ( DOWNPIXELS[0] == CLR_INVALID || DOWNPIXELS[1] == CLR_INVALID || DOWNPIXELS[2] == CLR_INVALID ) { std::cerr << "\u001b[31mERROR: Invalid pixel colour.\u001b[37m\n"; ReleaseDC( NULL, hdc ); return; }

		COLORREF RIGHTPIXELS[3];
		RIGHTPIXELS[0] = GetPixel( hdc, baseX + size / 1.2, baseY );
		RIGHTPIXELS[1] = GetPixel( hdc, baseX + size / 10, baseY + size / 2 );
		RIGHTPIXELS[2] = GetPixel( hdc, baseX + size / 10, baseY - size / 2 );
		if ( RIGHTPIXELS[0] == CLR_INVALID || RIGHTPIXELS[1] == CLR_INVALID || RIGHTPIXELS[2] == CLR_INVALID ) { std::cerr << "\u001b[31mERROR: Invalid pixel colour.\u001b[37m\n"; ReleaseDC( NULL, hdc ); return; }

		ReleaseDC( NULL, hdc );

		DIR d = INV;

		if (
			(int)GetRValue( UPPIXELS[0] ) < THRESHHOLD && (int)GetGValue( UPPIXELS[0] ) < THRESHHOLD && (int)GetBValue( UPPIXELS[0] ) < THRESHHOLD &&
			(int)GetRValue( UPPIXELS[1] ) < THRESHHOLD && (int)GetGValue( UPPIXELS[1] ) < THRESHHOLD && (int)GetBValue( UPPIXELS[1] ) < THRESHHOLD &&
			(int)GetRValue( UPPIXELS[2] ) < THRESHHOLD && (int)GetGValue( UPPIXELS[2] ) < THRESHHOLD && (int)GetBValue( UPPIXELS[2] ) < THRESHHOLD ) {

			d = UP;
		}
		else if (
			(int)GetRValue( LEFTPIXELS[0] ) < THRESHHOLD && (int)GetGValue( LEFTPIXELS[0] ) < THRESHHOLD && (int)GetBValue( LEFTPIXELS[0] ) < THRESHHOLD &&
			(int)GetRValue( LEFTPIXELS[1] ) < THRESHHOLD && (int)GetGValue( LEFTPIXELS[1] ) < THRESHHOLD && (int)GetBValue( LEFTPIXELS[1] ) < THRESHHOLD &&
			(int)GetRValue( LEFTPIXELS[2] ) < THRESHHOLD && (int)GetGValue( LEFTPIXELS[2] ) < THRESHHOLD && (int)GetBValue( LEFTPIXELS[2] ) < THRESHHOLD ) {

			d = LEFT;
		}
		else if (
			(int)GetRValue( DOWNPIXELS[0] ) < THRESHHOLD && (int)GetGValue( DOWNPIXELS[0] ) < THRESHHOLD && (int)GetBValue( DOWNPIXELS[0] ) < THRESHHOLD &&
			(int)GetRValue( DOWNPIXELS[1] ) < THRESHHOLD && (int)GetGValue( DOWNPIXELS[1] ) < THRESHHOLD && (int)GetBValue( DOWNPIXELS[1] ) < THRESHHOLD &&
			(int)GetRValue( DOWNPIXELS[2] ) < THRESHHOLD && (int)GetGValue( DOWNPIXELS[2] ) < THRESHHOLD && (int)GetBValue( DOWNPIXELS[2] ) < THRESHHOLD ) {

			d = DOWN;
		}
		else if (
			(int)GetRValue( RIGHTPIXELS[0] ) < THRESHHOLD && (int)GetGValue( RIGHTPIXELS[0] ) < THRESHHOLD && (int)GetBValue( RIGHTPIXELS[0] ) < THRESHHOLD &&
			(int)GetRValue( RIGHTPIXELS[1] ) < THRESHHOLD && (int)GetGValue( RIGHTPIXELS[1] ) < THRESHHOLD && (int)GetBValue( RIGHTPIXELS[1] ) < THRESHHOLD &&
			(int)GetRValue( RIGHTPIXELS[2] ) < THRESHHOLD && (int)GetGValue( RIGHTPIXELS[2] ) < THRESHHOLD && (int)GetBValue( RIGHTPIXELS[2] ) < THRESHHOLD ) {

			d = RIGHT;
		}

		if ( directions.back() != d ) {
			directions.push_back( d );

			if ( d != INV ) {
				std::cout << "Read " << DIRSTR[d - LEFT] << ".\n";
				current++;

				if ( round + 3 == current ) {

					current = 0;
					round++;

					std::this_thread::sleep_for( 0.5s );
					for ( auto d : directions ) {
						press( d );
					}
					directions.clear();
					directions.push_back( INV );

					if ( round == 5 ) {
						round = 0;

						std::this_thread::sleep_for( 2s );

						system( "cls" );
					}
				}
			}
		}
	}

} iOHandler;

void handleInput() {
	std::string in;
	for ( ;; ) {
		std::getline( std::cin, in );
		if ( in == "lock" ) {
			iOHandler.targetLockedIn = true;
			std::cout << "X: " << iOHandler.baseX << "; Y: " << iOHandler.baseY << ".\n";
			iOHandler.saveSettings();
		}
		else if ( in == "unlock" ) {
			iOHandler.targetLockedIn = false;
		}
		else if ( in == "reset" ) {
			iOHandler.reset();
		}
		else if ( in == "exit" ) {
			break;
		}
		else if ( in.find( "size" ) != -1 ) {
			in = in.substr( in.find( "size" ) + 4 );
			if ( in[0] == '+' ) {
				iOHandler.size += abs( std::stoi( in.substr( 1 ) ) );
				iOHandler.generate();
				iOHandler.saveSettings();
			}
			else if ( in[0] == '-' ) {
				unsigned int nr = abs( std::stoi( in.substr( 1 ) ) );
				if ( iOHandler.size <= nr ) continue;
				iOHandler.size -= nr;
				iOHandler.generate();
				iOHandler.saveSettings();
			}
			else if ( in[0] == '=' ) {
				iOHandler.size = abs( std::stoi( in.substr( 1 ) ) );
				iOHandler.generate();
				iOHandler.saveSettings();
			}
		}
		else {
			std::cerr << "\u001b[31mERROR: Unrecognized command.\u001b[37m\n";
		}
	}
}

int main() {

	iOHandler.generate();
	std::thread(
		[] {
			for ( ;; ) {
				iOHandler.read();
				std::this_thread::sleep_for( 5ms );
				iOHandler.draw();
			}}
	).detach();
	handleInput();
}
