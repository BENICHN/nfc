#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <complex>

using namespace std;
using namespace std::complex_literals;

using C = complex<double>;

void writeBMP(char *name, int w, int h, int **data)
{
    int x;
    int y;
    int c;
    int r;
    int g;
    int b;
    FILE *f;
    unsigned char *img = NULL;
    int filesize = 54 + 3 * w * h;

    img = (unsigned char *)malloc(3 * w * h);
    memset(img, 0, 3 * w * h);

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            x = i;
            y = (h - 1) - j;
            c = data[i][j];
            r = (c >> 16) & 0xFF;
            g = (c >> 8) & 0xFF;
            b = c & 0xFF;
            img[(x + y * w) * 3 + 2] = (unsigned char)(r);
            img[(x + y * w) * 3 + 1] = (unsigned char)(g);
            img[(x + y * w) * 3 + 0] = (unsigned char)(b);
        }
    }

    unsigned char bmpfileheader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
    unsigned char bmpinfoheader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};
    unsigned char bmppad[3] = {0, 0, 0};

    bmpfileheader[2] = (unsigned char)(filesize);
    bmpfileheader[3] = (unsigned char)(filesize >> 8);
    bmpfileheader[4] = (unsigned char)(filesize >> 16);
    bmpfileheader[5] = (unsigned char)(filesize >> 24);

    bmpinfoheader[4] = (unsigned char)(w);
    bmpinfoheader[5] = (unsigned char)(w >> 8);
    bmpinfoheader[6] = (unsigned char)(w >> 16);
    bmpinfoheader[7] = (unsigned char)(w >> 24);
    bmpinfoheader[8] = (unsigned char)(h);
    bmpinfoheader[9] = (unsigned char)(h >> 8);
    bmpinfoheader[10] = (unsigned char)(h >> 16);
    bmpinfoheader[11] = (unsigned char)(h >> 24);

    f = fopen(name, "wb");
    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);
    for (int i = 0; i < h; i++)
    {
        fwrite(img + (w * (h - i - 1) * 3), 3, w, f);
        fwrite(bmppad, 1, (4 - (w * 3) % 4) % 4, f);
    }

    free(img);
    fclose(f);
}

template <typename T>
T **create2DArray(int w, int h, T value)
{
    T **arr = new T *[h];
    for (int i = 0; i < h; i++)
    {
        T *row = new T[w];
        arr[i] = row;

        for (int j = 0; j < w; j++)
        {
            row[j] = value;
        }
    }
    return arr;
}

template <typename T>
T newton(int steps, T (*f)(T), T (*jf)(T), T x0)
{
    T x = x0;
    for (int i = 0; i < steps; i++)
    {
        x -= f(x) / jf(x);
    }
    return x;
}

C roots[] = {
    1. + 1i,
    1. - 1i,
    -1. + 1i,
    -1. - 1i,
};

int colors[] = {
    0x2c3e50,
    0x16a085,
    0xc0392b,
    0x8e44ad};

C f(C z)
{
    return (z - 1. + 1i) * (z - 1. - 1i) * (z + 1. + 1i) * (z + 1. - 1i);
}

C jf(C z)
{
    return (z - 1. + 1i) * (z - 1. - 1i) * (z + 1. + 1i) + (z - 1. + 1i) * (z - 1. - 1i) * (z + 1. - 1i) + (z - 1. + 1i) * (z + 1. + 1i) * (z + 1. - 1i) + (z - 1. - 1i) * (z + 1. + 1i) * (z + 1. - 1i);
}

template <typename T, int N>
int mini(T a[N])
{
    int r = -1;
    for (int i = 0; i < N; i++)
    {
        if (r == -1)
        {
            r = 0;
        }
        else if (a[i] < a[r])
        {
            r = i;
        }
    }
    return r;
}

int **nf(int w, int h, double s)
{
    double dists[4];
    int **fractal = create2DArray(w, h, 0);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            C z = complex<double>((double)i / s, (double)j / s);
            C zp = newton(50, f, jf, z);
            for (int i = 0; i < 4; i++)
            {
                dists[i] = norm(zp - roots[i]);
            }
            int im = mini<double, 4>(dists);
            fractal[i][j] = colors[im];
        }
    }
    return fractal;
}

int main(int argc, char const *argv[])
{
    int w = 512;
    int h = 512;
    char name[256];
    for (int i = 1; i <= 100; i++)
    {
        sprintf(name, "ah%d.bmp", i);
        writeBMP(name, w, h, nf(w, h, 2 * i * i));
    }
    return 0;
}