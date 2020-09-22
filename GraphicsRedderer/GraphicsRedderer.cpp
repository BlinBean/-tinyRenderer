#include <iostream>
#include<algorithm>
#include<vector>
#include "tgaimage.h"
#include"model.h"
#include"geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green= TGAColor(0, 255, 0, 255);



void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	bool steep = false;
	//如果高度>宽度（造成点间空隙过大），转置线段
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0,y0);
		std::swap(x1, y1);
		steep = true;
	}
	//确保第二个点的x>第一个点的x(确保结果为正数)
	if (x0>x1) {
		std::swap(x0,x1);
		std::swap(y0,y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	//float derror = std::abs(dy/(float)dx);//dy<dx//相当于斜率k
	float derror = std::abs(dy)*2;
	float error = 0;
	int y = y0;
	//画线
	for (int x = x0; x <= x1;x++) 
	{
		//float t = (x - x0) / (float)(x1 - x0);
		//int y = y0 * (1. - t) + y1 * t;
		//如果为转置状态的话，颠倒为yx
		if (steep) {
			image.set(y,x,color);
		}
		else {
			image.set(x,y,color);
		}
		//
		error += derror;//derror>0
		if (error > dx) {
			y += (y1>y0?1:-1);//y值上下一格
			error -= dx*2;
		}
	}

}

//void triangle( Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
//	line(t0.x,t0.y,t1.x,t1.y, image, color);
//	line(t1.x,t1.y,t2.x,t2.y, image, color);
//	line(t0.x,t0.y,t2.x,t2.y, image, color);
//}
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
	//排序
	if (t0.y > t1.y)
		std::swap(t0,t1);
	if (t0.y > t2.y)
		std::swap(t0, t2);
	if (t1.y > t2.y)
		std::swap(t1,t2);

	int total_height = t2.y - t0.y;

	//底部着色//起点t0
	for (int y = t0.y; y <= t1.y;y++) {
		int segment_height = t1.y - t0.y;//t1-t0
		//减去底部顶点
		float alpha = (float)(y - t0.y) / total_height;//长
		float beta = (float)(y - t0.y) / segment_height;//短

		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = t0 + (t1 - t0) * beta;

		if (A.x > B.x)
			std::swap(A,B);
		for (int j = A.x; j < B.x;j++) {
			image.set(j, y, color);
		}

		//image.set(A.x, y, red);
		//image.set(B.x, y, green);
	}
	//顶部着色//起点t1//
	for (int y = t1.y; y <= t2.y; y++) {
		int segment_height = t2.y - t1.y;//t2-t1

		//减去t0和t1
		float alpha = (float)(y - t0.y) / total_height;//长
		float beta = (float)(y- t1.y) / segment_height;//短

		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = t1 + (t2 - t1) * beta;

		if (A.x > B.x)
			std::swap(A, B);
		for (int j = A.x; j < B.x; j++) {
			image.set(j, y, color);
		}
	}

	//line(t0.x, t0.y, t1.x, t1.y, image, color);
	//line(t1.x, t1.y, t2.x, t2.y, image, color);
	//line(t0.x, t0.y, t2.x, t2.y, image, color);
}


int main(int argc, char** argv) {

	TGAImage image(500, 500, TGAImage::RGB);

	//add point and line
	//image.set(52, 41, red);
	//line(13,20,80,40,image,white);
	//line(20,13,40,80,image,red);
	//line(80,40,13,20,image,red);

	//import model
	//Model* model = new Model("D:\\School\\Scripts\\Graph\\GraphicsC++\\GraphicsRedderer\\obj\\african_head\\african_head.obj");
	//for (int i = 0; i < model->nfaces(); i++) {
	//	std::vector<int> face = model->face(i, 0);
	//	for (int j = 0; j < 3; j++) {
	//		Vec3f v0 = model->vert(face[j]);
	//		Vec3f v1 = model->vert(face[(j + 1) % 3]);
	//		int x0 = (v0.x + 1.) * 500 / 2.;
	//		int y0 = (v0.y + 1.) * 500 / 2.;
	//		int x1 = (v1.x + 1.) * 500 / 2.;
	//		int y1 = (v1.y + 1.) * 500 / 2.;
	//		line(x0, y0, x1, y1, image, white);
	//	}
	//}

	Vec2i t0[3] = {Vec2i(10,70),Vec2i(50,160),Vec2i(70,80)};
	Vec2i t1[3] = {Vec2i(180,50),Vec2i(150,1),Vec2i(70,180)};
	Vec2i t2[3] = {Vec2i(180,150),Vec2i(120,160),Vec2i(130,180)};
	triangle(t0[0],t0[1],t0[2],image,red);
	triangle(t1[0],t1[1],t1[2],image,white);
	triangle(t2[0],t2[1],t2[2],image,green);
	


	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("D:\\School\\Scripts\\Graph\\GraphicsC++\\GraphicsRedderer\\out\\output.tga");
	

	return 0;
}



