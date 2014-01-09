
#ifndef LIB_BMP32_HPP
#define LIB_BMP32_HPP

#define LIBBMP_VERSION  1000

namespace libbmp32 {

#include <cstdlib>
#include <iostream>
#include <cstdint>



//------------------------------------------------------------------------------------------
//  �r�b�g�}�b�v�t�@�C���w�b�_
#pragma pack(1)

struct BitmapFileHeader {
    uint16_t type_;
    uint32_t size_;
    uint16_t reserved1_;
    uint16_t reserved2_;
    uint32_t offbits_;
};

struct BitmapInfoHeader{
  uint32_t size_;
  int32_t width_;
  int32_t heigth_;
  uint16_t planes_;
  uint16_t bit_count_;
  uint32_t compression_;
  uint32_t size_image_;
  int32_t pels_per_meter_x_;
  int32_t pels_per_meter_y_;
  uint32_t clr_used_;
  uint32_t clr_important_;
};

struct RgbQuad{
  uint8_t blue_;
  uint8_t green_;
  uint8_t red_;
  uint8_t reserved_;
};

#pragma pack()




//------------------------------------------------------------------------------------------
//  �J���[�l
struct Color
{
    Color()
        : r_(0)
        , g_(0)
        , b_(0)
    {
    }

    Color(
        const int r,
        const int g,
        const int b
    )   : r_(r)
        , g_(g)
        , b_(b)
    {
    }
    //  Color * s
    Color operator *( const float s ) const {
        return Color(r_ * s, g_ * s, b_ * s);
    }


    uint8_t r_;
    uint8_t g_;
    uint8_t b_;
};


//------------------------------------------------------------------------------------------
//  �s�N�`��
class Image
{
    typedef Image   self_t;

private:
    //  ctor.
    Image()
        : width_(0)
        , height_(0)
        , r_(nullptr)
        , g_(nullptr)
        , b_(nullptr)
        , a_(nullptr)
    {
    }

    //  dtor.
    ~Image() 
    {
        delete[] r_;
        delete[] g_;
        delete[] b_;
        delete[] a_;
    }

public:

    int width_;
    int height_;
    uint8_t* r_;
    uint8_t* g_;
    uint8_t* b_;
    uint8_t* a_;

public:
    /**
     *  �C���X�^���X����.
     */
    static self_t* createImage() {
        self_t* image = new self_t;
        return image;
    }

    /**
     *  �C���X�^���X����.
     */
    static self_t* createImage(
        const size_t width,   //  ��
        const size_t height   //  ����
    ){
        self_t* image = new self_t;
        image->setupImage(
            width,
            height
        );
          
        return image;          
    }

    /**
     *  �j��.
     */
    void destroy() {
        delete this;
    }

    /**
     *  ���̏o��.
     */
    void dump() {
        ::std::cout << "x:" << width_ << std::endl;
        ::std::cout << "y:" << height_ << std::endl;
    }

    /**
     *  �s�N�Z���l�擾.
     */
    void getPixel(
        Color* out,
        const int x,
        const int y
    ) {
        int pixel_index = x + (y * width_);
        out->r_ = r_[pixel_index];
        out->g_ = g_[pixel_index];
        out->b_ = b_[pixel_index];
    }


    /**
     *  �s�N�Z���l���Z.
     */
    void addPixelColor(
        const int x,
        const int y,
        const Color& color
    ){
        //  �C���[�W�͈̔͊O�ւ̃v���b�g�͖���
        if (x < 0 || x >= width_){
            return;
        }
        if (y < 0 || y >= height_){
            return;
        }

        int pixel_index = x + (y * width_);

        if (r_[pixel_index] + color.r_ < 256) {
            r_[pixel_index] += color.r_;
        }
        else {
            r_[pixel_index] = 255;
        }

        if (g_[pixel_index] + color.g_ < 256) {
            g_[pixel_index] += color.g_;
        }
        else {
            g_[pixel_index] = 255;
        }

        if (b_[pixel_index] + color.b_ < 256) {
            b_[pixel_index] += color.b_;
        }
        else {
            b_[pixel_index] = 255;
        }
    }

    /**
     *  �s�N�Z���l�w��.
     */
    void plot(
        const int x,
        const int y,
        const Color& color
    ) {
        plot(x, y, color.r_, color.g_, color.b_);
    }

    /**
     *  �s�N�Z���l�w��.
     */
    void plot(
        const int x,
        const int y,
        const int r,
        const int g,
        const int b
    ) {
        //  �C���[�W�͈̔͊O�ւ̃v���b�g�͖���
        if (x < 0 || x >= width_){
            return;
        }
        if (y < 0 || y >= height_){
            return;
        }

        int pixel_index = x + (y * width_);
        r_[pixel_index] = r;
        g_[pixel_index] = g;
        b_[pixel_index] = b;
    }


    /**
     *  �h��Ԃ�.
     */
    void fill(
        const Color& color
    ) {
        fill(color.r_, color.g_, color.b_);
    }

    /**
     *  �h��Ԃ�.
     */
    void fill(
        const int r,
        const int g,
        const int b
    ) {
        size_t pixel_num = width_ * height_;
        for (size_t i = 0; i < pixel_num; ++i) {
            r_[i] = r;
            g_[i] = g;
            b_[i] = b;            
        }
    }


private:
    /**
     *  �T�C�Y�ݒ�.
     */
    void setupImage(
        const size_t width, 
        const size_t height
    ) {

        //  �T�C�Y�ݒ�
        width_ = width;
        height_ = height;

        //  �̈�m��
        size_t pixel_num = height_ * width_;
        delete[] r_;
        r_ = new unsigned char[pixel_num];

        delete[] g_;
        g_ = new unsigned char[pixel_num];
        
        delete[] b_;
        b_ = new unsigned char[pixel_num];

    }

};

/**
 *  �r�b�g�}�b�v�t�@�C���w�b�_�\�z.
 */
void setupBitmapFileHeader(
    BitmapFileHeader* const file_header,
    const int img_size
){
    file_header->type_ = *(uint16_t*)"BM";  // TODO �萔�A�N�Z�X����
    file_header->size_ = img_size + sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    file_header->reserved1_ = 0;
    file_header->reserved2_ = 0;
    file_header->offbits_ = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
}

/**
 *  �r�b�g�}�b�v�C���t�H�w�b�_�\�z.
 */
void setupBitmapInfoHeader(
    BitmapInfoHeader* const info_header,
    const int x,
    const int y,
    const int img_size
){
    info_header->size_ = sizeof(BitmapInfoHeader);
    info_header->width_ = x;
    info_header->heigth_ = y;
    info_header->planes_ = 1;
    info_header->bit_count_ = 24;
    info_header->compression_ = 0;
    info_header->size_image_ = img_size;
    info_header->pels_per_meter_x_ = 0;
    info_header->pels_per_meter_y_ = 0;
    info_header->clr_used_ = 0;
    info_header->clr_important_ = 0;
}



//------------------------------------------------------------------------------------------
//  �r�b�g�}�b�v�����o��. fstream ver.
bool serializeBmp(
    std::ofstream* file, 
    const Accessor* const accessor
){
    if(file == nullptr || image == nullptr) {
        return false;
    }

    //  ----------------------------------
    //  �w�b�_�쐬
    int x = accessor->width_;
    int y = accessor->height_;
    int padding = (x * 3 + 3) / 4 * 4 - x * 3;      // 32bit���E�����ɂ��p�f�B���O
    int img_size = (x * 3 + padding) * y;           // �o�͂����ꑝ�f�[�^�T�C�Y

    //  �t�@�C���w�b�_
    BitmapFileHeader file_header;
    setupBitmapFileHeader(&file_header, img_size);

    //  �C���t�H�w�b�_
    BitmapInfoHeader info_header;
    setupBitmapInfoHeader(
        &info_header,
        x,
        y,
        img_size
    );

    //  ----------------------------------
    //  �w�b�_�o��
    file->write((char*)&file_header, sizeof(BitmapFileHeader));
    file->write((char*)&info_header, sizeof(BitmapInfoHeader));

    //  ���e�o��
    file->write((char*)accessor->data_, img_size);

    //  ����
    return true;

}


//------------------------------------------------------------------------------------------
/**
 *  �r�b�g�}�b�v�̐������`�F�b�N.
 */
bool isVaildBmp(
    const BitmapFileHeader* const file_header,
    const BitmapInfoHeader* const info_header
){
    if (file_header->type_ != *(uint16_t*)"BM") { // TODO �萔����ɕς���
        return false;
    }
    if (info_header->size_ != sizeof(BitmapInfoHeader)) {
        return false;
    }
    if (info_header->compression_ != 0) {
        return false;
    }

    int pallet_size = file_header->offbits_ - sizeof(BitmapFileHeader) - sizeof(BitmapInfoHeader);
    int img_size = info_header->size_image_;     // �C���[�W�T�C�Y
    int x = info_header->width_;                 //  �C���[�W�̕�
    int y = info_header->heigth_;                // �C���[�W�̍���
    int color_bit = info_header->bit_count_;     // �P�F������̃r�b�g��


    //  �w�b�_�̖������m�F
    if (color_bit  != 24 
        && pallet_size != sizeof(RgbQuad) * (1 << color_bit)) {
        return false;
    }

    if (color_bit  == 0) {
        return false;
    }

    if (x <= 0 || y <= 0) {
        return false;
    }

    if (img_size != file_header->size_ - file_header->offbits_) {
        return false;
    }

    if (img_size != (x * color_bit + 31) / 32 * 4 * y) {
        return false;
    }

    return true;
}


//------------------------------------------------------------------------------------------
//  �r�b�g�}�b�v�ǂݍ���. fstream ver.
bool deserializeBmp(
    std::ifstream* file,
    Accessor* accessor
) {

    if(file == nullptr) {
        return false;
    }

    //  �w�b�_�擾
    file->read((char *)(&accessor->file_header_), sizeof(BitmapFileHeader));
    if( file->bad() ) {
        //  �ǂݍ��ݎ��s
        return nullptr;
    }

    //  �t�@�C���w�b�_
    file->read((char *)(&accessor->info_header_), sizeof(BitmapInfoHeader));
    if( file->bad() ) {
        //  �ǂݍ��ݎ��s
        return nullptr;
    }


    if (!isVaildBmp(&accessor->file_header_, &accessor->info_header_)) {
        //  �s���ȃf�[�^
        return false;
    }

    //  �C���[�W�f�[�^�擾
    int img_size = info_header.size_image_;
    uint8_t* buf = (uint8_t*)malloc(img_size);
    uint8_t* buf_top = buf;

    if (buf == nullptr) {
        //  �ǂݍ��ݎ��s
        return false;
    }


    file->read((char *)buf, img_size);

    //  �r�b�g�}�b�v����C���[�W�f�[�^����
    accessor->buf_ = buf;

    //   �Еt��
    return true;
}


}   // namespace libbmp32


#endif // LIB_BMP32_HPP

