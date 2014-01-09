
#ifndef LIB_BMP32_HPP
#define LIB_BMP32_HPP

#define LIBBMP_VERSION      1001
#define LIBBMP_SIGNATURE    19778

#include <cstdlib>
#include <cstdint>
#include <fstream>


namespace libbmp24 {


//------------------------------------------------------------------------------------------
//  �r�b�g�}�b�v�t�@�C���w�b�_


class Bitmap {

public:
    struct FileHeader {
        uint32_t size_;
        uint16_t reserved1_;
        uint16_t reserved2_;
        uint32_t offbits_;

        void setup(const int image_size) {
            size_ = image_size + sizeof(FileHeader) + sizeof(InfoHeader);
            reserved1_ = 0;
            reserved2_ = 0;
            offbits_ = sizeof(FileHeader) + sizeof(InfoHeader);
        }
    };

    struct InfoHeader{
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

        void setup(
            const int width,
            const int height
        ){
            int padding = (width * 3 + 3) / 4 * 4 - width * 3;    // 32bit���E�����ɂ��p�f�B���O
            int img_size = (width * 3 + padding) * height;        // �o�͂����ꑝ�f�[�^�T�C�Y

            size_ = sizeof(InfoHeader);
            width_ = width;
            heigth_ = height;
            planes_ = 1;
            bit_count_ = 24;
            compression_ = 0;
            size_image_ = img_size;
            pels_per_meter_x_ = 0;
            pels_per_meter_y_ = 0;
            clr_used_ = 0;
            clr_important_ = 0;
        }

        void dump() const {
            printf("size = %d\n", size_);
            printf("width = %d\n", width_);
            printf("heigth = %d\n", heigth_);
            printf("planes = %d\n", planes_);
            printf("bit_count = %d\n", bit_count_);
            printf("compression = %d\n", compression_);
            printf("size_image = %d\n", size_image_);
            printf("pels_per_meter_x = %d\n", pels_per_meter_x_);
            printf("pels_per_meter_y = %d\n", pels_per_meter_y_);
            printf("clr_used = %d\n", clr_used_);
            printf("clr_important = %d\n", clr_important_);
        }
    };

    Bitmap()
        : file_header_()
        , info_header_()
        , data_(nullptr)
    {}

    ~Bitmap() {
        delete[] data_;
    }


    int32_t getWidth() const {
        return info_header_.width_;
    }

    int32_t getHeight() const {
        return info_header_.heigth_;
    }

    uint32_t getImageSize() const {
        return info_header_.size_image_;
    }

    const uint8_t* getData() const {
        return data_;
    }

    void dump() const {
        info_header_.dump();
    }

    void fill(int r, int g, int b) {
        int width = getWidth();
        int height = getHeight();
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                int pixel_idx = x + (y * width);
                int data_idx = pixel_idx * 3;

                data_[data_idx + 0] = static_cast<uint8_t>(b);
                data_[data_idx + 1] = static_cast<uint8_t>(g);
                data_[data_idx + 2] = static_cast<uint8_t>(r);
            }
        }
    }

private:
    FileHeader file_header_;
    InfoHeader info_header_;
    uint8_t* data_;

public:
    //-------------------------------------------
    //  �r�b�g�}�b�v����
    void createBitmap(
        int width,
        int height
    ) {
        info_header_.setup(width, height);
        int image_size = getImageSize();
        file_header_.setup(image_size);
        delete[] data_;
        data_ = new uint8_t[image_size];
    }


    //------------------------------------------------------------------------------------------
    //  �r�b�g�}�b�v�����o��.
    bool serialize(
        std::ofstream& file
    ) const {
        
        //  ----------------------------------
        //  �w�b�_�o��
        uint16_t bm_signature = LIBBMP_SIGNATURE;
        file.write((char*)&bm_signature, 2);
        file.write((char*)&file_header_, sizeof(FileHeader));
        file.write((char*)&info_header_, sizeof(InfoHeader));

        //  ���e�o��
        file.write((char*)data_, getImageSize());

        //  ����
        return true;

    }


    //------------------------------------------------------------------------------------------
    /**
     *  �r�b�g�}�b�v�̐������`�F�b�N.
     */
    bool isVaildate() const {
        if (info_header_.size_ != sizeof(InfoHeader)) {
            return false;
        }
        if (info_header_.compression_ != 0) {
            return false;
        }

        int pallet_size = file_header_.offbits_ - sizeof(FileHeader) - sizeof(InfoHeader);
        int img_size = info_header_.size_image_;     // �C���[�W�T�C�Y
        int x = info_header_.width_;                 //  �C���[�W�̕�
        int y = info_header_.heigth_;                // �C���[�W�̍���
        int color_bit = info_header_.bit_count_;     // �P�F������̃r�b�g��


        //  �w�b�_�̖������m�F
        if (color_bit  != 24) {
            return false;
        }

        if (color_bit  == 0) {
            return false;
        }

        if (x <= 0 || y <= 0) {
            return false;
        }

        if (img_size != file_header_.size_ - file_header_.offbits_) {
            return false;
        }

        if (img_size != (x * color_bit + 31) / 32 * 4 * y) {
            return false;
        }

        return true;
    }


    //------------------------------------------------------------------------------------------
    //  �r�b�g�}�b�v�ǂݍ���. fstream ver.
    bool deserialize (
        std::ifstream& file
    ) {

        //  �V�O�l�`���m�F
        uint16_t signature = 0;
        file.read((char*)(&signature), 2);

        if (signature != LIBBMP_SIGNATURE) {
            //  �r�b�g�}�b�v�t�@�C���ł͂Ȃ�
            return false;
        }

        //  �w�b�_�擾
        file.read((char *)(&file_header_), sizeof(FileHeader));
        if( file.bad() ) {
            //  �ǂݍ��ݎ��s
            return false;
        }

        //  �t�@�C���w�b�_
        file.read((char *)(&info_header_), sizeof(InfoHeader));
        if (file.bad()) {
            //  �ǂݍ��ݎ��s
            return false;
        }


        if (!isVaildate()) {
            //  �s���ȃf�[�^
            return false;
        }

        //  �C���[�W�f�[�^�擾
        int img_size = getImageSize();
        delete[] data_;
        data_ = new uint8_t[img_size];

        if (!data_) {
            //  �ǂݍ��ݎ��s
            return false;
        }

        file.read((char *)data_, img_size);

        return true;
    }
};



}   // namespace libbmp32


#endif // LIB_BMP32_HPP

