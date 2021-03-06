#include <stdio.h>
#include <windows.h>
#include <math.h>
#define WIDTHBYTES(bits) (((bits)+31)/32*4)		// 영상의 가로줄은 4 바이트의 배수
#define BYTE unsigned char

int main()
{
	// Gray-scale 비트맵 영상 생성
	errno_t err;
	FILE *outfile;
	double a, b;
	int i, j;

	BITMAPFILEHEADER hf;  // 파일정보헤드 변수 선언
	BITMAPINFOHEADER hInfo;  // 영상정보헤드 변수 선언
	RGBQUAD hRGB[256];  // 팔레트 정보를 위한 배열(흑백파일)

	int rwsize = WIDTHBYTES(8 * 256);
	BYTE *OutImg = new BYTE[rwsize * 256];  // 저장할 영상메모리 할당
	BYTE *GrayImg = new BYTE[rwsize * 256];

	// 영상헤드 설정
	hInfo.biSize = sizeof(BITMAPINFOHEADER);
	hInfo.biWidth = 256;
	hInfo.biHeight = 256;
	hInfo.biPlanes = 1;  // 비트 플레인 수
	hInfo.biBitCount = 8;  // 픽셀당 비트수
	hInfo.biCompression = 0;  // 압축 유무
	hInfo.biSizeImage = rwsize * 256;  // 영상의 크기
	hInfo.biXPelsPerMeter = 0;  // 가로 해상도
	hInfo.biYPelsPerMeter = 0;  // 세로 해상도
	hInfo.biClrUsed = hInfo.biClrImportant = 0;  // 팔레트의 색깔 수, 비트맵 표시에 반드시 필요한 색깔 수

	// 파일헤드 설정
	hf.bfType = 0x4D42;
	hf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(hRGB);
	hf.bfSize = hf.bfOffBits + hInfo.biSizeImage;

	// 그레이스케일 영상의 팔레트 설정
	for (int i = 0; i < 256; i++)
	{
		hRGB[i].rgbBlue = i;
		hRGB[i].rgbGreen = i;
		hRGB[i].rgbRed = i;
	}


	// 원의 반지름에 따른 명도값 설정
	for (i = 0; i < hInfo.biHeight; i++)
	{
		for (j = 0; j < hInfo.biWidth; j++)
		{
			a = pow((i - 128), 2);
			b = pow((j - 128), 2);

			if (a + b < pow(40, 2))
				GrayImg[i * rwsize + j] = 0;
			else if (a + b < pow(80, 2))
				GrayImg[i * rwsize + j] = 64;
			else if (a + b < pow(120, 2))
				GrayImg[i * rwsize + j] = 128;
			else
				GrayImg[i * rwsize + j] = 200;

			// 좌측 상단 정사각형
			if ((i <= 32) && (j <= 32))
				GrayImg[i * rwsize + j] = 0;
		}
	}

	// 거꾸로 저장된 이미지 반전시키기
	for (i = 0; i < hInfo.biHeight; i++)
	{
		for (j = 0; j < hInfo.biWidth; j++)
		{
			OutImg[i * hInfo.biWidth + j] = GrayImg[(hInfo.biHeight - i - 1) * rwsize + j];
		}
	}

	// 영상 출력
	err = fopen_s(&outfile, "sample gray image.bmp", "wb");  // 출력할 파일을 오픈
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);  // 파일헤드 출력
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);  // 영상헤드 출력
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);  // 팔레트 출력
	fwrite(OutImg, sizeof(char), hInfo.biSizeImage, outfile);  // 영상데이터 출력
	fclose(outfile);

	delete[] OutImg;
	delete[] GrayImg;

	return 0;
}