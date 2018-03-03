// Gif_To_Mp4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


Mat Gif_To_Mat(FIBITMAP* fiBmp, const FREE_IMAGE_FORMAT fif);
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message);
bool Gif_Load(const string &filename);
bool Jpg_To_Video();
bool Show_Video();

unsigned char bpp;
BYTE *bits;
bool Load_flag;
int width;
int height;
FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
Mat Src_Gif;


int _tmain(int argc, _TCHAR* argv[])
{
	FreeImage_Initialise();
	Load_flag = Gif_Load("Gif_2.gif");

	if (Load_flag == TRUE)
	{
		Jpg_To_Video();
		Show_Video();
		cout << "load file successful!" << endl;
	}
	return 0;
}

//-------------  GIF�ļ�������
//

bool Gif_Load(const string &filename)
{
	FIBITMAP *dib = 0;
	FIMULTIBITMAP *bitmap = 0;
	FIBITMAP * pFrame;

	fif = FreeImage_GetFileType(filename.c_str(), 0);
	if (fif == FIF_UNKNOWN)						fif = FreeImage_GetFIFFromFilename(filename.c_str());
	if (fif == FIF_UNKNOWN)						return false;
	if (FreeImage_FIFSupportsReading(fif))		dib = FreeImage_Load(fif, filename.c_str());
	if (!dib)									return false;//dib Load failed	

	//bpp = FreeImage_GetBPP(dib);
	bits = (BYTE*)FreeImage_GetBits(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);

	cout << "Load The File:   " << filename.c_str()  << endl;
	cout << "The File's width: " << width << endl;
	cout << "The File's height: " << height << endl;

	if ((bits == 0) || (width == 0) || (height == 0))		return false;

	bitmap = FreeImage_OpenMultiBitmap(fif, filename.c_str(), 0, 0, 1, GIF_DEFAULT);
	if (bitmap == NULL)
	{
		cout << "BitMap == Null" << endl;
		return FALSE;
	}
	
	int count = FreeImage_GetPageCount(bitmap);//��ȡ֡����

	for (int i = 0; i <=count; i++)
	{
		pFrame = FreeImage_LockPage(bitmap, i);
		//cout << "pFrame:" << pFrame << endl;
		Src_Gif = Gif_To_Mat(pFrame, fif);
		
		string Src_Gif_Name = to_string(i);
		imwrite(Src_Gif_Name + ".jpg", Src_Gif);
		FreeImage_UnlockPage(bitmap, pFrame, 1);
	}

	FreeImage_Unload(dib);
	FreeImage_DeInitialise();
	Load_flag = TRUE;
	return Load_flag;
}

/*
����ǰ�ļ����е� ��.jpg�� ����Ϊ��Ƶ�ļ���

*/

bool Jpg_To_Video()
{

	VideoWriter video("output.avi", CV_FOURCC('M', 'P', '4', '2'), 25.0, Size(150, 131));
	String File_Name = "*.jpg";
	vector <String> fn;
	glob(File_Name, fn, false);//�����ļ��е�ͼƬ/�ļ�
	size_t size = fn.size();
	cout << "Jpg_To_Video size:" << size << endl;
	cout << "��ʼ��ͼƬ�ļ�д����Ƶ" << endl;
	for (size_t i = 0; i < size; i++)
	{
		Mat image = imread(fn[i]);
		//imshow(to_string(i), image);
		//resize(image, image, Size(640, 480));
		video.write(image);
	}

	cout << "д�� �ɹ�!" << endl;

	return TRUE;
}

/*

��ʾ���ɵ���Ƶ�ļ���

*/

bool Show_Video()
{
	cout << "Show The Video.." << endl;

	VideoCapture video_capture("output.avi");
	if (!video_capture.isOpened())		return FALSE;


	double totalFrameNumber = video_capture.get(CV_CAP_PROP_FRAME_COUNT);
	cout << "������Ƶ��" << totalFrameNumber << "֡" << endl;
	//���ÿ�ʼ֡()
	long frameToStart = 1;
	video_capture.set(CV_CAP_PROP_POS_FRAMES, frameToStart);
	cout << "�ӵ�" << frameToStart << "֡��ʼ��" << endl;

	//���ý���֡
	int frameToStop = totalFrameNumber;

	if (frameToStop < frameToStart)
	{
		cout << "����֡С�ڿ�ʼ֡��������󣬼����˳���" << endl;
		return FALSE;
	}
	else
	{
		cout << "����֡Ϊ����" << frameToStop << "֡" << endl;
	}

	//��ȡ֡��
	double rate = video_capture.get(CV_CAP_PROP_FPS);
	cout << "֡��Ϊ:" << rate << endl;

	//����һ���������ƶ�ȡ��Ƶѭ�������ı���
	bool stop = false;
	//����ÿһ֡��ͼ��
	Mat frame;
	//��ʾÿһ֡�Ĵ���
	namedWindow("Extracted frame");
	//��֡��ļ��ʱ��:
	double delay = rate;

	//����whileѭ����ȡ֡
	//currentFrame����ѭ�����п��ƶ�ȡ��ָ����֡��ѭ�������ı���
	long currentFrame = frameToStart;


	while (!stop)
	{
		//��ȡ��һ֡
		if (!video_capture.read(frame))
		{
			cout << "��ȡ��Ƶ����" << endl;
			return FALSE;
		}

		imshow("Extracted frame", frame);

		cout << "���ڶ�ȡ��" << currentFrame << "֡" << endl;

		//waitKey(int delay=0)��delay �� 0ʱ����Զ�ȴ�����delay>0ʱ��ȴ�delay����
		//��ʱ�����ǰû�а�������ʱ������ֵΪ-1�����򷵻ذ���
		int c = waitKey(delay);

		//����ESC���˳���Ƶ��֡����ʾ
		if ((char)c == 27 || currentFrame > frameToStop)
		{
			stop = true;
		}
		//���°������ͣ���ڵ�ǰ֡���ȴ���һ�ΰ���
		if (c >= 0)
		{
			waitKey(0);
		}
		currentFrame++;
	}
	//�ر���Ƶ�ļ�
	video_capture.release();
	waitKey(0);
	return TRUE;
}
//-------------  Conversion Gif to Mat
//

Mat Gif_To_Mat(FIBITMAP* fiBmp, const FREE_IMAGE_FORMAT fif)
{
	if (fiBmp == NULL || fif != FIF_GIF)
	{
		return Mat();
	}

	//int width = FreeImage_GetWidth(fiBmp);
	//int height = FreeImage_GetHeight(fiBmp);
	
	BYTE intensity;
	BYTE* PIintensity = &intensity;
	if (FreeImage_GetBPP(fiBmp) != 8)
		fiBmp = FreeImage_ConvertTo8Bits(fiBmp);

	RGBQUAD* pixels = new RGBQUAD;
	pixels = FreeImage_GetPalette(fiBmp);

	Mat img = Mat::zeros(height, width, CV_8UC3);

	uchar *p;

	for (int i = 0; i < height; i++)
	{
		p = img.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			FreeImage_GetPixelIndex(fiBmp, j, height - i, PIintensity);
			p[3 * j] = pixels[intensity].rgbBlue;
			p[3 * j + 1] = pixels[intensity].rgbGreen;
			p[3 * j + 2] = pixels[intensity].rgbRed;
		}
	}

	return img;
}


//--------------  ErrorHandler  --------------
// FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
// In your main program ��
//FreeImage_SetOutputMessage(FreeImageErrorHandler);


void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
	cout << "\n***" << endl;
	if (fif != FIF_UNKNOWN)
	{
		cout  << FreeImage_GetFormatFromFIF(fif) << "\n" << endl;

	}
	cout << message << endl;
	cout << "***\n" << endl;
}

