#include <iostream>
#include<algorithm>
#include<vector>
#include "tgaimage.h"
#include"model.h"
#include"geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green= TGAColor(0, 255, 0, 255);

const int width = 200;
const int height = 200;


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

//计算重心坐标
Vec3f barycentric(Vec2i *pts ,Vec2i P) {
	Vec3f u = cross(
		Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]),
		Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1])
	);
	if (std::abs(u[2]) < 1)	return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
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

	//------------------第一版-------------------
	//底部着色//起点t0
	//for (int y = t0.y; y <= t1.y;y++) {
	//	int segment_height = t1.y - t0.y;//t1-t0
	//	//减去底部顶点
	//	float alpha = (float)(y - t0.y) / total_height;//长
	//	float beta = (float)(y - t0.y) / segment_height;//短

	//	Vec2i A = t0 + (t2 - t0) * alpha;
	//	Vec2i B = t0 + (t1 - t0) * beta;

	//	if (A.x > B.x)
	//		std::swap(A,B);
	//	for (int j = A.x; j < B.x;j++) {
	//		image.set(j, y, color);
	//	}

	//	//image.set(A.x, y, red);
	//	//image.set(B.x, y, green);
	//}
	////顶部着色//起点t1//
	//for (int y = t1.y; y <= t2.y; y++) {
	//	int segment_height = t2.y - t1.y;//t2-t1

	//	//减去t0和t1
	//	float alpha = (float)(y - t0.y) / total_height;//长
	//	float beta = (float)(y- t1.y) / segment_height;//短

	//	Vec2i A = t0 + (t2 - t0) * alpha;
	//	Vec2i B = t1 + (t2 - t1) * beta;

	//	if (A.x > B.x)
	//		std::swap(A, B);
	//	for (int j = A.x; j < B.x; j++) {
	//		image.set(j, y, color);
	//	}
	//}
	//------------END------------------

	for (int i = 0; i < total_height; i++) {
		//t1和t0在同一高度or i值大于下面两点的差值
		bool sencond_half = i > t1.y - t0.y || t1.y == t0.y;
		//如果大于下面两点的差值的话,用上面的点减第二高的点//否则得到下面两点的差值
		int segment_height = sencond_half ? t2.y - t1.y : t1.y - t0.y;
		
		float alpha = (float)i / total_height;
		float beta = (float)(i - (sencond_half ?t1.y-t0.y:0)) / segment_height;

		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B=sencond_half?t1+(t2-t1)*beta:t0+(t1-t0)*beta;
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++)
			image.set(j,t0.y+i,color);
	}

	//line(t0.x, t0.y, t1.x, t1.y, image, color);
	//line(t1.x, t1.y, t2.x, t2.y, image, color);
	//line(t0.x, t0.y, t2.x, t2.y, image, color);
}

void triangle(Vec2i *pts,TGAImage &image ,TGAColor color) {
	//获得画布边界
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);
	//限制区间
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	//获得图形边界
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			//每个点和边界对比
			bboxmin[j] = std::max(0,std::min(bboxmin[j],pts[i][j]));
			bboxmax[j] = std::min(clamp[j],std::max(bboxmax[j],pts[i][j]));
		}
	}
	Vec2i P;
	//填充图像
	for (P.x == bboxmin.x; P.x <= bboxmax.x;P.x++) {
		for (P.y == bboxmin.y; P.y <= bboxmax.y;P.y++) {
			Vec3f bc_screen = barycentric(pts, P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			image.set(P.x, P.y, color);
		}
	}
}

int main(int argc, char** argv) {

	//TGAImage image(500, 500, TGAImage::RGB);

	//----------------------

	//--------point-------
	//add point and line
	//image.set(52, 41, red);

	//-------line-----------
	//line(13,20,80,40,image,white);
	//line(20,13,40,80,image,red);
	//line(80,40,13,20,image,red);

	//---------import model----------------
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

	//--------Draw triangle--------------
	//Vec2i t0[3] = {Vec2i(10,70),Vec2i(50,160),Vec2i(70,80)};
	//Vec2i t1[3] = {Vec2i(180,50),Vec2i(150,1),Vec2i(70,180)};
	//Vec2i t2[3] = {Vec2i(180,150),Vec2i(120,160),Vec2i(130,180)};
	//triangle(t0[0],t0[1],t0[2],image,red);
	//triangle(t1[0],t1[1],t1[2],image,white);
	//triangle(t2[0],t2[1],t2[2],image,green);

	TGAImage frame(200, 200, TGAImage::RGB);
	Vec2i pts[3] = { Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160) };
	triangle(pts,frame,red);
	frame.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	frame.write_tga_file("D:\\School\\Scripts\\Graph\\GraphicsC++\\GraphicsRedderer\\out\\output.tga");

	//image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	//image.write_tga_file("D:\\School\\Scripts\\Graph\\GraphicsC++\\GraphicsRedderer\\out\\output.tga");
	

	return 0;
}



