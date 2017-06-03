#include "PathTracer.h"

int PathTracer::width = 800, PathTracer::height = 600;
GLubyte* PathTracer::frame = NULL;

PathTracer::PathTracer()
{
}


PathTracer::~PathTracer()
{
	for (unordered_map<string, Material*>::iterator i = Materials.begin(); i != Materials.end(); i++)
		delete i->second;
}

//optional
bool PathTracer::loadParameterFile(string name) {
	ifstream ifs(name);
	if (!ifs.is_open()) {
		cout << " [loadParameterFile] No parameter file! ignored." << endl;
		return false;
	}
	string tmp_s;
	while (!ifs.eof()) {
		ifs >> tmp_s;
		if (!tmp_s.compare("obj_file")) {
			ifs >> tmp_s;
			if (!loadObject(tmp_s)) return false;
		}
		else if (!tmp_s.compare("parameter_file")) {
			ifs >> tmp_s;
			if (!loadParameters(tmp_s)) return false;
		}
	}
	return true;
}

//optional
bool PathTracer::loadParameters(string name) {
	ifstream ifs(name);
	if (!ifs.is_open()) {
		cout << " [loadParameter] No parameter file! ignored." << endl;
		return false;
	}

	string tmp_s;
	int tmp_i, tmp_i2;
	float tmp_f;
	float tmp_fs[10];

	while (!ifs.eof()) {
		ifs >> tmp_s;
		if (!tmp_s.compare("sample_num")) {
			ifs >> tmp_i;
			sample_num = tmp_i;
		}
		else if (!tmp_s.compare("pixel_size")) {
			ifs >> tmp_i >> tmp_i2;
			setPixelSize(tmp_i, tmp_i2);
		}
		else if (!tmp_s.compare("max_depth")) {
			ifs >> tmp_i;
			max_depth = tmp_i;
		}
		else if (!tmp_s.compare("env_intensity")) {
			ifs >> tmp_f;
			env_intensity = tmp_f;
		}
		else if (!tmp_s.compare("light_intensity")) {
			ifs >> tmp_f;
			light_intensity = tmp_f;
		}
		else if (!tmp_s.compare("camera")) {
			for (int i = 0; i < 9; i++)
				ifs >> tmp_fs[i];
			setCamera(Vector3f(tmp_fs[0], tmp_fs[1], tmp_fs[2]), Vector3f(tmp_fs[3], tmp_fs[4], tmp_fs[5]), Vector3f(tmp_fs[6], tmp_fs[7], tmp_fs[8]));	
		}
	}
	ifs.close();

	return true;
}

void PathTracer::loadMaterial(string name)
{
	ifstream ifs(name);
	if (!ifs.is_open()) {
		cout << " [loadMaterial] Open file error!" << endl;
		return;
	}

	string tmp_s;
	float tmp_f1, tmp_f2, tmp_f3;
	Material* current_m = new Material();

	while (!ifs.eof()) {
		ifs >> tmp_s;
		if (!tmp_s.compare("newmtl"))
		{
			ifs >> tmp_s;
			current_m = new Material();
			Materials[tmp_s] = current_m;
			current_m->name = tmp_s;
		}
		else if (!tmp_s.compare("illum")) {
			ifs >> current_m->illum;
		}
		else if (!tmp_s.compare("Ka")) {
			ifs >> tmp_f1 >> tmp_f2 >> tmp_f3;
			current_m->Ka = Vector3f(tmp_f1, tmp_f2, tmp_f3);
		}
		else if (!tmp_s.compare("Kd")) {
			ifs >> tmp_f1 >> tmp_f2 >> tmp_f3;
			current_m->Kd = Vector3f(tmp_f1, tmp_f2, tmp_f3);
		}
		else if (!tmp_s.compare("Ks")) {
			ifs >> tmp_f1 >> tmp_f2 >> tmp_f3;
			current_m->Ks = Vector3f(tmp_f1, tmp_f2, tmp_f3);
		}
		else if (!tmp_s.compare("Ns")) {
			ifs >> current_m->Ns;
		}
		else if (!tmp_s.compare("Tr")) {
			ifs >> current_m->Tr;
		}
		else if (!tmp_s.compare("Ni")) {
			ifs >> current_m->Ni;
		}
		else {
			cout << "[load material] Error. Not correct input" << endl;
			break;
		}
	}
}

bool PathTracer::loadObject(string name)
{
	ifstream ifs(name);
	if (!ifs.is_open()) {
		cout << " [loadObject] Open file error!" << endl;
		return false;
	}

	vertexs.clear();
	objects.clear();

	string tmp_s, tmp_s2;
	float tmp_f1, tmp_f2, tmp_f3;
	int tmp_i;
	Object* current_o = NULL;
	while (!ifs.eof()) {
		ifs >> tmp_s;
		if (!tmp_s.compare("mtllib")) {
			ifs >> tmp_s;
			name = name.substr(0, name.find_last_of('/') + 1) + tmp_s;
			loadMaterial(name);
		}
		else if (!tmp_s.compare("usemtl")) {
			current_o = new Object();
			current_o->type = ObjectType::MESH;
			ifs >> tmp_s;
			current_o->material = Materials[tmp_s];
#if USE_LIGHT_NAME
			if (!tmp_s.compare(light_name))
				current_o->material->is_light = true;
			else
				current_o->material->is_light = false;
#endif
			objects.push_back(current_o);
		}
		else if (!tmp_s.compare("v")) {
			ifs >> tmp_f1 >> tmp_f2 >> tmp_f3;
			vertexs.push_back(Vector3f(tmp_f1, tmp_f2, tmp_f3));
		}
		else if (!tmp_s.compare("f")) {
			vector<int> tmp_v;
			getline(ifs, tmp_s);
			const char* temp = tmp_s.c_str();

			while (temp = strchr(temp, ' '))
			{
				if (sscanf(temp, "%d", &tmp_i) != 1)
					break;
				tmp_i--;
				tmp_v.push_back(tmp_i);
				temp += strspn(temp, " ");

#ifdef USE_BOUNDING_BOX
				Vector3f v = vertexs[tmp_i];
				current_o->setLimits(v);
#endif
			}
			current_o->faces.push_back(tmp_v);
		}
		else if (!tmp_s.compare("sphere")) {
			current_o = new Sphere();
			current_o->type = ObjectType::SPHERE;
			ifs >> tmp_s;
			current_o->material = Materials[tmp_s];
			ifs >> tmp_f1 >> tmp_f2 >> tmp_f3;
			((Sphere*)current_o)->center = Vector3f(tmp_f1, tmp_f2, tmp_f3);
			ifs >> tmp_f1;
			((Sphere*)current_o)->radius = tmp_f1;
#if USE_LIGHT_NAME
			if (!tmp_s.compare(light_name))
				current_o->material->is_light = true;
			else
				current_o->material->is_light = false;
#endif
			objects.push_back(current_o);
		}
		else getline(ifs, tmp_s);
	}
	ifs.close();

#ifdef USE_BOUNDING_BOX
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->setBoundingBox(vertexs);
	}
#endif

#if !USE_LIGHT_NAME
	for (int i = 0; i < objects.size(); i++) {
		if (objects[i]->material->Ka[0] > 1e-5)
			objects[i]->material->is_light = true;
		else
			objects[i]->material->is_light = false;
	}
#endif

	return true;
}


void PathTracer::setCamera(Vector3f pos, Vector3f front, Vector3f up) {
	camera.Pos = pos;
	camera.Front = front.normalized();
	camera.Up = up.normalized();
}

void PathTracer::setPixelSize(int width, int height) {
	this->width = width;
	this->height = height;
}

// calculate the pixel origin position and the increments in both directions
void PathTracer::calculatePixel() {
	float tx = tanf(fov / 2.0f*M_PI / 180.0f)*near_clip_z;
	float aspect_ratio = width*1.0f / height;
	float ty = tx / aspect_ratio;
	Vector3f x_dir = camera.Front.cross(camera.Up).normalized();
	pixel_origin_pos = camera.Pos + camera.Front*near_clip_z - x_dir*tx - camera.Up*ty;
	pixel_x_inc = x_dir * tx / width * 2;
	pixel_y_inc = camera.Up * ty / height * 2;
}

void PathTracer::intersect(Intersection& inter, Vector3f pos, Vector3f dir) {
	inter.is_intersected = false;
	inter.min_t = DBL_MAX;
	for (int i = 0; i < objects.size(); i++)
		objects[i]->intersect(inter, vertexs, pos, dir); // virtual function
	return;
}

Vector3f PathTracer::refractedRay(const Vector3f& V, const Vector3f &N, float Ni, bool& inside) const {
	float nr = !inside ? 1 / Ni : Ni;
	inside = !inside;
	Vector3f T = (nr* N.dot(V) - sqrt(1 - nr*nr*(1 - pow(N.dot(V), 2))))*N - nr*V;
	return T;
}

//get a random ray in semisphere
Vector3f PathTracer::randomReflectedRay(const Vector3f &N, double given_phi) const {
	double theta = 2.0* M_PI * ives::rand();
	double phi = given_phi >1e-5 ? given_phi : asin(ives::rand()) + 0.01;
	Vector3f u = fabs(N.x()) > 0.5 ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0);
	u = u.cross(N).normalized();
	Vector3f v = N.cross(u);
	Vector3f d = (u*cos(theta) + v*sin(theta)) * cos(phi) + N*sin(phi);
#ifndef NDEBUG
	if (d.dot(N) < -0.01)
		cout << "[randomReflectedRay] error direction occur !" << endl;
#endif
	return d.normalized();
}

//to get better specular, add weight
Vector3f PathTracer::weightedRandomReflectedRay(const Vector3f& V, const Vector3f &N, const Material* m) const {
	float Ns = m->Ns;
	if (Ns <= 1.0 || ives::rand()< 1.5 / sqrt(Ns))
		return randomReflectedRay(N);
	Vector3f S = 2 * N * N.dot(V) - V;
	double phi1 = M_PI / 2.0 * (1 - 1 / sqrt(Ns));
	double phi2 = acos(S.dot(N) / S.norm());
	double min_phi = max(phi1, phi2);
	double phi = min_phi + ives::rand() * (M_PI / 2.0 - min_phi); // random range: [min_phi,PI/2]
	return randomReflectedRay(S, phi);
}


Vector3f PathTracer::pathTracing(Vector3f pos, Vector3f dir, int depth, bool inside) {
	using namespace ives;

	if (depth >= max_depth) {
#ifndef NDEBUG
		cout << depth << " reach max_depth !!!" << endl;
#endif
		return Vector3f(0, 0, 0);
	}

	Intersection inter;
	intersect(inter, pos, dir); // the returned normal vector is already normalized

	if (!inter.is_intersected) {
#ifndef NDEBUG
		cout << depth << "  hit nothing !!!" << endl;
#endif
		return env_color * env_intensity;
	}

	if (inter.obj->material->is_light) {
#ifndef NDEBUG
		cout << depth << "  hit light !!!" << endl;
#endif
		return light_color * light_intensity;
	}

	//phong model: Ie = Ka*Ia + Ii(Kd(N*L) + Ks(V*R)^Ns)
	//refraction model : see the document
	Vector3f N;
	if (inter.normal.dot(dir) > 0)
		N = -inter.normal;
	else
		N = inter.normal;
	float Ns = inter.obj->material->Ns;
	Vector3f V = -dir;

	float Tr = inter.obj->material->Tr;  //transparency
	bool refraction = ives::rand() < Tr ? true : false;

	Vector3f L;
	if (Tr > 1e-4 && refraction)
		L = refractedRay(V, N, inter.obj->material->Ni, inside);
	else
		L = weightedRandomReflectedRay(V, N, inter.obj->material);

#ifndef NDEBUG
	cout << depth << " hit object:" << inter.obj->material->name << " #pos:" << outVec(inter.pos) << " #t: " << inter.min_t << " #N:" << outVec(N) << "  #nextdir:" << outVec(L) << " #inside:" << inside << endl;
#endif

	Vector3f Ii = pathTracing(inter.pos, L, depth + 1, inside);

	if (Ii[0] + Ii[1] + Ii[2] < 1e-6)
	{
#ifndef NDEBUG
		cout << depth << " #Ii:" << outVec(Ii) << " too small! discard!!!" << endl;
#endif
		return Vector3f(0, 0, 0);
	}

	Vector3f Ie;
	if (!refraction) { //phong
		Vector3f Ka = inter.obj->material->Ka;
		Vector3f Ia = env_color*env_intensity;
		Vector3f Kd = inter.obj->material->Kd;
		Vector3f Ks = inter.obj->material->Ks;
		Vector3f R = 2 * N * N.dot(L) - L;
		Ie = ives::rgbMultiple(Ka, Ia) + ives::rgbMultiple(Ii, Kd*N.dot(L) + Ks* pow(V.dot(R), Ns));
	}
	else Ie = Ii; //refraction

#ifndef NDEBUG
	cout << depth << " #Ii:" << outVec(Ii) << "-> #Ie:" << outVec(Ie) << endl;
#endif
	return Ie;
}

void PathTracer::render()
{
	using namespace ives;

	cout << "【Start tracing!】" << endl << fixed << setprecision(3);
	time_t start, stop;
	time(&start);               // Start execution timer
	srand(time(0));

	calculatePixel();

	int percent = 0;
#pragma omp parallel for schedule(dynamic)       // OpenMP
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++)
		{
			Vector3f color(0, 0, 0);
			for (int k = 0; k < sample_num; k++)
			{
				float x_rand = ives::rand();
				float y_rand = ives::rand();
				Vector3f p = pixel_origin_pos + pixel_x_inc*(i + x_rand) + pixel_y_inc*(j + y_rand);
				Vector3f d = (p - camera.Pos).normalized();
#ifndef NDEBUG
				cout << "pathTracing start. No. >>>> " << i*width + j << " - " << k << " <<<<" << endl << "#pos:" << outVec(p) << " #dir:" << outVec(d) << endl;
#endif
				Vector3f c = pathTracing(p, d, 0);
#ifndef NDEBUG
				cout << endl;
#endif
				color += c;
			}
			color /= sample_num;
			setColor(i, j, color);
		}
		if (i + 1 > percent) {
			percent = i + 1;
			printf("\r %.2f%%", percent * 100.0 / width);
		}
	}
	printf("\r %.2f%%", 100.0);
	time(&stop);
	double diff = difftime(stop, start);
	cout << endl << "【End tracing!】total time:" << diff << " seconds." << endl;

	//OpenGL相关
	glutDisplayFunc(display);
	//glutIdleFunc(NULL);
}

inline void PathTracer::setColor(int x, int y, Vector3f color)
{
	if (color[0] > 1) color[0] = 1;
	if (color[1] > 1) color[1] = 1;
	if (color[2] > 1) color[2] = 1;
	*(frame + (width*y + x) * 3) = (GLubyte)(color.x() * 255);
	*(frame + (width*y + x) * 3 + 1) = (GLubyte)(color.y() * 255);
	*(frame + (width*y + x) * 3 + 2) = (GLubyte)(color.z() * 255);
}

//OpenGL绘制函数
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glRasterPos2i(0, 0);
	glDrawPixels(PathTracer::width, PathTracer::height, GL_RGB, GL_UNSIGNED_BYTE, PathTracer::frame);
	glutSwapBuffers();
}


void  PathTracer::glInit(const char* name) {
	//OpenGL初始化
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(PathTracer::width, PathTracer::height);
	glutCreateWindow(name);
	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, PathTracer::width << 6, 0, PathTracer::height << 6);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	frame = (GLubyte*)malloc(width*height * 3 * sizeof(GLubyte));
}
