#include "tgaimage.h"
#include <iostream>
#include"model.h"
#include"geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);


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

int main(int argc, char** argv) {

	TGAImage image(500, 500, TGAImage::RGB);
	//image.set(52, 41, red);
	//line(13,20,80,40,image,white);
	//line(20,13,40,80,image,red);
	//line(80,40,13,20,image,red);
	Model* model = new Model("D:\\School\\Scripts\\Graph\\GraphicsC++\\GraphicsRedderer\\obj\\african_head\\african_head.obj");
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i, 0);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.) * 500 / 2.;
			int y0 = (v0.y + 1.) * 500 / 2.;
			int x1 = (v1.x + 1.) * 500 / 2.;
			int y1 = (v1.y + 1.) * 500 / 2.;
			line(x0, y0, x1, y1, image, white);
		}
	}




	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("D:\\School\\Scripts\\Graph\\GraphicsC++\\GraphicsRedderer\\out\\output.tga");
	

	return 0;
}



