#pragma once 

#include "../config.h"
#include <stdbool.h>

// begin of copy from nanovg.h

union Color {
	float rgba[4];
	struct {
		float r, g, b, a;
	} s;
};

/**
 * @brief �F��RGBA�e256�K���ŕ\��
 */
typedef union Color Color;

struct Paint {
	float xform[6];
	float extent[2];
	float radius;
	float feather;
	Color innerColor;
	Color outerColor;
	int image;
};

/**
 * @brief �`��p�^�[��
 */
typedef struct Paint Paint;


/**
 * @biref ��]����
 */
enum Winding {
	WINDING_CCW = 1,			//! �����v���
	WINDING_CW = 2,				//! ���v���
};

/**
 * @biref �`�悷��`��̓���
 */
enum Solidity {
	SOLIDITY_SOLID = 1,			//! �`�悷��`��͖ʂƂ��Ē�`
	SOLIDITY_HOLE = 2,			//! �`�悷��`��͌��Ƃ��Ē�`
};

/**
 * �]�k�F
 * �p�X��p���ăh�[�i�c�̂悤�Ȍ��̊J�����`���h��Ԃ��ĕ`�悷����@
 *
 * Video.BeginPath();
 *
 * Video.Circle(120, 120, 250);        // �h�[�i�c�̖{�̂ƂȂ�`���ݒ�
 * Video.PathWinding(SOLIDITY_SOLID);  // ���O�ɐݒ肵���`��`���ʂɐݒ�
 * 
 * Video.Circle(120, 120, 50);        // �h�[�i�c�̌��ƂȂ�`���ݒ�
 * Video.PathWinding(SOLIDITY_HOLE);   // ���O�ɐݒ肵���`��`������ɐݒ�
 *
 * Video.FillColor(Video.RGB(255,255,255));
 * Video.Fill();
 *
 * Video.ClosePath();
 */

/**
* @biref �����̏I�[�̏���
*/
enum LineCap {
	CAP_BUTT,	//! ���̏I�_�łԂ؂�
	CAP_ROUND,	//! ���̕��̔����̒����𔼌a�Ƃ��锼�~
	CAP_SQUARE,	//! ���̕��̔����̒��������͂ݏo��
	CAP_BEVEL,	//! �}�C�^�[�����𖳌��ɂ��Č�_�܂ŉ����o��
	CAP_MITER,	//! �}�C�^�[������L���ɂ���
};

/**
 * (��Ƀe�L�X�g��)����w��t���O.
 */
enum Align {
	ALIGN_LEFT   = 1 << 0,	//! [��������] ���񂹁i�f�t�H���g�l)
	ALIGN_CENTER = 1 << 1,	//! [��������] ������
	ALIGN_RIGHT  = 1 << 2,	//! [��������] �E��

	ALIGN_TOP      = 1 << 3,	//! [��������] ���
	ALIGN_MIDDLE   = 1 << 4,	//! [��������] ����
	ALIGN_BOTTOM   = 1 << 5,	//! [��������] ����
	ALIGN_BASELINE = 1 << 6,	//! [��������] �x�[�X���C���񂹁i�f�t�H���g�l)
};

/**
 * @brief �t�H���g�̃O���t�ʒu���
 */
struct GlyphPosition {
	const char* str;	//! �O���t�ɑΉ����镶���ʒu
	float x;			//! �_���O���t��X���W
	float minx, maxx;	//! �O���t�`���X���W�̍ŏ��l�ƍő�l
};
typedef struct GlyphPosition GlyphPosition;

struct TextRow {
	const char* start;	// Pointer to the input text where the row starts.
	const char* end;	// Pointer to the input text where the row ends (one past the last character).
	const char* next;	// Pointer to the beginning of the next row.
	float width;		// Logical width of the row.
	float minx, maxx;	// Actual bounds of the row. Logical with and bounds can differ because of kerning and some parts over extending.
};
typedef struct TextRow TextRow;

enum ImageFlags {
	IMAGE_GENERATE_MIPMAPS = 1 << 0,	// �摜��ǂݍ��ލۂɃ~�b�v�}�b�v�f�[�^�𐶐�����B�k���`��𑽗p����摜���Ƃ�⑬�x���オ�邩������Ȃ��B
	IMAGE_REPEATX = 1 << 1,				// �摜�̉������͈͂���E���ĕ`�悷��ہA�摜�����ɖ����ɘA�����Ă���悤�ɕ`�悷��B�iImagePattern()�ŉ摜���𒴂���T�C�Y�̃p�^�[����`�悷��ۂɁA�摜���J��Ԃ����ɕ��ׂĕ`�悷��j
	IMAGE_REPEATY = 1 << 2,				// �摜�̏c�����͈͂���E���ĕ`�悷��ہA�摜���c�ɖ����ɘA�����Ă���悤�ɕ`�悷��B�iImagePattern()�ŉ摜�����𒴂���T�C�Y�̃p�^�[����`�悷��ۂɁA�摜���J��Ԃ����ɕ��ׂĕ`�悷��j
	IMAGE_FLIPY = 1 << 3,				// �摜���㉺���]���ĕ`�悷��B�i�ꕔ�̉摜�`���͌Â��c�[���ŏo�͂���Ə㉺���]�ŉ摜���i�[����Ă���ꍇ�����邽�߁j
	IMAGE_PREMULTIPLIED = 1 << 4,		// ���`�����l�������摜�̍��������Ƀv���}���`�v���C�h������p����B�iAdobe AfterEffect����v���}���`�v���C�h�����ŏo�͂����摜��TIFF�`���ȂǂŕK�v�j
#if defined(UNOFFICIAL_HACK_ENABLE_NEAREST_RESIZE)
	IMAGE_RESIZE_NEAREST = 1 << 5,		// �摜�̊g��k���Ƀj�A���X�g�l�C�o�[(nearest neighbor)�@���g���B�h�b�g�G�ȂǂɎg���B
#endif
};

// end of copy 

/**
 * @brief �`�施��
 */
extern const struct __tagVideoAPI {
	/**
	* @brief VideoAPI������������
	* @retval true  �������ɐ���
	* @retval false �������Ɏ��s
	* @note ���C�u�����̓����ŗ��p����Ă��邽�ߒʏ�͗��p�҂��Ăяo���K�v�͖����B
	*/
	bool(*Initialize)(void);

	/**
	 * @brief VideoAPI�̏I���������s��
	 * @note ���C�u�����̓����ŗ��p����Ă��邽�ߒʏ�͗��p�҂��Ăяo���K�v�͖����B
	 */
	void(*Finalize)(void);

	/**
	 * @brief Video.SetWindowTitle()�֐���UTF8��p��
	 */
	void(*SetWindowTitleUTF8)(const char *title);

	/**
	 * @brief �E�B���h�E�^�C�g����ݒ肷��
	 * @param title �E�B���h�E�^�C�g��
	 */
	void(*SetWindowTitle)(const char *title);

	/**
	 * @brief ��ʃT�C�Y��ݒ肷��
	 * @param width  �� 
	 * @param height ����
	 *
	 * �P�ʂ̓s�N�Z��
	 */
	void(*SetSize)(int width, int height);

	/**
	 * @brief �Q�[���̏�ԍX�V�E�`�揈���Ȃǂ��\�Ȃ�܂ő҂�
	 * @return ���̊֐��͕֋X����true��Ԃ�
	 *
	 * while (Video.Drawing()) { �Q�[���̏�ԍX�V�E�`�揈�� } �̂悤�ɗp����
	 * ��ʂ�����ꂽ�ꍇ�ɂ̓v���O�����͎����I�ɏI������
	 */
	bool(*Drawing)(void);

	/**
	 * @brief �v���O�����N��������̌o�ߕb�����擾
	 * @return �o�ߕb�����P�b=1.0�Ƃ��Ď擾�B 1�~���b=1.0/1000
	 */
	double(*GetTime)(void);

	/**
	 * @brief Video.Drawing()����ʏ����ɗp����F�� color �ɐݒ肷��
	 * @param color  �ݒ肷��F
	 */
	void(*SetClearColor)(Color color);

	/**
	 * @brief Vide.Drawing()���g�킸�ɑS�Ă̕`����Ǘ�����ꍇ�ɗp����`�揈���̊J�n
	 * @param windowWidth  ��ʕ�
	 * @param windowHeight ��ʍ�
	 * @param devicePixelRatio �t���[���o�b�t�@�T�C�Y/��ʃT�C�Y �ŋ��߂��T�C�Y�䗦
	 * @note ���C�u�����̓����ŗ��p����Ă��邽�ߒʏ�͗��p�҂��Ăяo���K�v�͖����B
	 */
	void(*BeginFrame)(int windowWidth, int windowHeight, float devicePixelRatio);

	/**
	 * @brief Video.Drawing()���g�킸�ɑS�Ă̕`����Ǘ�����ꍇ�ɗp����`�揈���̃L�����Z��
	 * @note ���C�u�����̓����ŗ��p����Ă��邽�ߒʏ�͗��p�҂��Ăяo���K�v�͖����B
	 */
	void(*CancelFrame)(void);

	/**
	 * @brief Video.Drawing()���g�킸�ɑS�Ă̕`����Ǘ�����ꍇ�ɗp����`�揈���̏I��
	 * @note ���C�u�����̓����ŗ��p����Ă��邽�ߒʏ�͗��p�҂��Ăяo���K�v�͖����B
	 */
	void(*EndFrame)(void);
	
	/**
	 * @brief �F��RGB�e256�K���Ŏw�肵�č쐬����BA(�������x)��255�Œ�ƂȂ�
	 * @param r �Ԃ̊K��
	 * @param g �΂̊K��
	 * @param b �̊K��
	 * @return �쐬�����F�f�[�^
	 */
	Color(*RGB)(unsigned char r, unsigned char g, unsigned char b);
	
	/**
	 * @brief �F��RGBA�e256�K���Ŏw�肵�č쐬����B
	 * @param r �Ԃ̊K��
	 * @param g �΂̊K��
	 * @param b �̊K��
	 * @param a �s�����x(255�͊��S�s�����A0�͊��S����)
	 * @return �쐬�����F�f�[�^
	 */
	Color(*RGBA)(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	
	/**
	 * @brief ���݂̕`��ݒ���R�s�[���ăX�^�b�N�ɐς�
	 * @note  Video.Drawing()���Ăяo���ƕ`��ݒ���X�^�b�N��Ԃ����Z�b�g�����
	 */
	void(*Save)(void);

	/**
	 * @brief ���݂̕`��ݒ���X�^�b�N������o���Č��݂̐ݒ�ɂ���
	 * @note  Video.Drawing()���Ăяo���ƕ`��ݒ���X�^�b�N��Ԃ����Z�b�g�����
	 */
	void(*Restore)(void);

	/**
	 * @brief ���݂̕`��ݒ�������l�Ƀ��Z�b�g����
	 */
	void(*Reset)(void);

	/**
	 * @brief Video.Stroke()�ŕ`�悷����̐F��ݒ肷��
	 * @param color ���̐F
	 */
	void(*StrokeColor)(Color color);

	/**
	 * @brief Video.Stroke()�ŕ`�悷����̃p�^�[����ݒ肷��
	 * @param paint ���̃p�^�[��
	 * @note  �p�^�[����Video.LinearGradient()/BoxGradient()/RadialGradient()/ImagePattern()�ō쐬����
	 */
	void(*StrokePaint)(Paint paint);

	/**
	 * @brief Video.Fill()�ŕ`�悷��h��Ԃ��̐F��ݒ肷��
	 * @param color �h��Ԃ��̐F
	 */
	void(*FillColor)(Color color);

	/**
	 * @brief Video.Stroke()�ŕ`�悷��h��Ԃ��̃p�^�[����ݒ肷��
	 * @param paint �h��Ԃ��̃p�^�[��
	 * @note  �p�^�[����Video.LinearGradient()/BoxGradient()/RadialGradient()/ImagePattern()�ō쐬����
	 */
	void(*FillPaint)(Paint paint);
	
	/**
	 * @brief �܂���̐ڍ��ӏ��̃}�C�^�[���E�i�܂���ӏ���ʎ�肵�Ȃ��Ő�点��j�������w�肷��
	 * @param limit �}�C�^�[����
	 * @note  �}�C�^�[�����ɂ��Ẳ���� http://www.htmq.com/canvas/miterLimit.shtml ���Q��
	 */
	void(*MiterLimit)(float limit);

	/**
	 * @brief Video.Stroke()�ŕ`�悷����̕���ݒ肷��
	 * @param paint ���̕�
	 */
	void(*StrokeWidth)(float size);

	/**
	 * @brief Video.Stroke()�ŕ`�悷����̏I�[�̌`����w�肷��
	 * @param cap ���̏I�[�̌`��
	 * @note  �I�[�̌`��ɂ��Ă� enum LineCap �̉�����Q��
	 */
	void(*LineCap)(enum LineCap cap);

	/**
	* @brief �܂���̐ڑ��`����w�肷��
	* @param join ���̏I�[�̌`��ibevel, round, miter�̂����ꂩ�j
	* @note  �I�[�̌`��ɂ��Ă� enum LineCap �̉�����Q��
	*/
	void(*LineJoin)(enum LineCap join);

	/**
	 * @brief �`��S�̂̕s�����x��0.0�`1.0�Ŏw��
	 * @param alpha �s�����x(1.0�����S�s����, 0.0�����S����)
	 */
	void(*GlobalAlpha)(float alpha);

	/**
	 * @brief ���݂̕ό`�ݒ�����Z�b�g����i�p���ϊ��s��ɒP�ʍs���ݒ�j
	 * | 1, 0, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 */
	void(*ResetTransform)(void);

	/**
	 * @brief ���݂̕ό`�ݒ�ɔC�ӂ�2x3�p���ϊ��s���ݒ肷��
	 * @param a,b,c,d,e,f �s��v�f
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | a, c, e |
	 * | b, d, f |
	 * | 0, 0, 1 |
	 */
	void(*Transform)(float a, float b, float c, float d, float e, float f);

	/**
	 * @brief ���݂̕ό`�ݒ�Ɉړ�(x, y)���E����K�p
	 * @param x �������ړ���
	 * @param y �c�����ړ���
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, 0, x |
	 * | 0, 1, y |
	 * | 0, 0, 1 |
	 */
	void(*Translate)(float x, float y);

	/**
	 * @brief ���݂̕ό`�ݒ�� ���v���� angle ���W�A���̉�]���E����K�p
	 * @param angle ���v���ł̉�]
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | c,-s, 0 | 
	 * | s, c, 0 |
	 * | 0, 0, 1 |
	 * c = cos(angle), s = sin(angle)
	 */
	void(*Rotate)(float angle);

	/**
	 * @brief ���݂̕ό`�ݒ�� angle ���W�A����X���ɉ������X�Εό`���E����K�p
	 * @param angle �X�Ίp�x
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, t, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 */
	void(*SkewX)(float angle);

	/**
	 * @brief ���݂̕ό`�ݒ�� angle ���W�A����Y���ɉ������X�Εό`���E����K�p
	 * @param angle �X�Ίp�x
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, 0, 0 |
	 * | t, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 */
	void(*SkewY)(float angle);

	/**
	 * @brief ���݂̕ό`�ݒ�� �� x �{, �c y �{�̃X�P�[�����E����K�p
	 * @param x �������X�P�[��
	 * @param y �c�����X�P�[��
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | x, 0, 0 |
	 * | 0, y, 0 |
	 * | 0, 0, 1 |
	 */
	void(*Scale)(float x, float y);

#if defined(UNOFFICIAL_HACK_ENABLE_TRANSFORM_WORLD)
	/**
	 * @brief ���݂̕ό`�ݒ�Ɉړ�(x, y)��������K�p
	 * @param x �������ړ���
	 * @param y �c�����ړ���
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, 0, x |
	 * | 0, 1, y |
	 * | 0, 0, 1 |
	 */
	void(*TranslateWorld)(float x, float y);

	/**
	 * @brief ���݂̕ό`�ݒ�� ���v���� angle ���W�A���̉�]��������K�p
	 * @param angle ���v���ł̉�]
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | c,-s, 0 | 
	 * | s, c, 0 |
	 * | 0, 0, 1 |
	 * c = cos(angle), s = sin(angle)
	 */
	void(*RotateWorld)(float angle);

	/**
	 * @brief ���݂̕ό`�ݒ�� angle ���W�A����X���ɉ������X�Εό`��������K�p
	 * @param angle �X�Ίp�x
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, t, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 */
	void(*SkewXWorld)(float angle);

	/**
	 * @brief ���݂̕ό`�ݒ�� angle ���W�A����Y���ɉ������X�Εό`��������K�p
	 * @param angle �X�Ίp�x
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, 0, 0 |
	 * | t, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 */
	void(*SkewYWorld)(float angle);

	/**
	 * @brief ���݂̕ό`�ݒ�� �� x �{, �c y �{�̃X�P�[����������K�p
	 * @param x �������X�P�[��
	 * @param y �c�����X�P�[��
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | x, 0, 0 |
	 * | 0, y, 0 |
	 * | 0, 0, 1 |
	 */
	void(*ScaleWorld)(float x, float y);
#endif

	/**
	 * @brief ���݂̕ό`�ݒ��z��xform�Ɏ��o��
	 * @param xform 6�v�f�ȏ�̔z��B 
	 * @note  �z��v�f�ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | a, c, e |
	 * | b, d, f |
	 * | 0, 0, 1 |
	 * xform = [a,b,c,d,e,f]
	 */
	void(*CurrentTransform)(float* xform);

	/**
	 * @brief �z�� dst �ɒP�ʍs���ݒ�
	 * @param dst 6�v�f�ȏ�̔z��B 
	 * @note  �z��v�f�ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, 0, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 * dst = [1,0,0,1,0,0]
	 */
	void(*TransformIdentity)(float* dst);

	/**
	 * @brief �z�� dst �Ɉړ��s��(x, y)��ݒ�
	 * @param dst 6�v�f�ȏ�̔z��B 
	 * @param x �������ړ���
	 * @param y �c�����ړ���
	 * @note  �z��v�f�ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, 0, x |
	 * | 0, 1, y |
	 * | 0, 0, 1 |
	 * dst = [1,0,0,1,x,y]
	 */
	void(*TransformTranslate)(float* dst, float sx, float sy);

	/**
	 * @brief �z�� dst �Ɏ��v���� angle ���W�A���̉�]�s���ݒ�
	 * @param dst 6�v�f�ȏ�̔z��B 
	 * @param angle ���v���ł̉�]
	 * @note  �z��v�f�ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | c,-s, 0 | 
	 * | s, c, 0 |
	 * | 0, 0, 1 |
	 * c = cos(angle), s = sin(angle)
	 * dst = [c,s,-s,c,0,0]
	 */
	void(*TransformRotate)(float* dst, float a);

	/**
	 * @brief �z�� dst ��angle ���W�A����X���ɉ������X�Εό`�s���ݒ�
	 * @param dst 6�v�f�ȏ�̔z��B 
	 * @param angle �X�Ίp�x
	 * @note  �z��v�f�ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, t, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 * dst = [1,0,t,1,0,0]
	 */
	void(*TransformSkewX)(float* dst, float a);

	/**
	 * @brief �z�� dst ��angle ���W�A����Y���ɉ������X�Εό`�s���ݒ�
	 * @param dst 6�v�f�ȏ�̔z��B 
	 * @param angle �X�Ίp�x
	 * @note  �z��v�f�ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | 1, 0, 0 |
	 * | t, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 * dst = [1,t,0,1,0,0]
	 */
	void(*TransformSkewY)(float* dst, float a);

	/**
	 * @brief �z�� dst �ɉ� x �{, �c y �{�̃X�P�[���s���ݒ�
	 * @param dst 6�v�f�ȏ�̔z��B 
	 * @param x �������X�P�[��
	 * @param y �c�����X�P�[��
	 * @note  �e�����ƍs��v�f�̑Ή��͈ȉ��̒ʂ�
	 * | x, 0, 0 |
	 * | 0, y, 0 |
	 * | 0, 0, 1 |
	 * dst = [x,0,0,y,0,0]
	 */
	void(*TransformScale)(float* dst, float sx, float sy);

	/**
	 * @brief �z�� dst �Ɣz�� src ���s��ƌ��Ȃ���dst * src ���s���B���ʂ�dst�Ɋi�[�����B
	 * @param dst 6�v�f�ȏ�̔z��B
	 * @param src 6�v�f�ȏ�̔z��B
	 * @note �z��v�f�ƍs��v�f�̑Ή��� Transform() �֐��̂��̂Ɠ���
	 */
	void(*TransformMultiply)(float* dst, const float* src);

	/**
	 * @brief �z�� dst �Ɣz�� src ���s��ƌ��Ȃ���src * dst ���s���B���ʂ�dst�Ɋi�[�����B
	 * @param dst 6�v�f�ȏ�̔z��B
	 * @param src 6�v�f�ȏ�̔z��B
	 * @note �z��v�f�ƍs��v�f�̑Ή��� Transform() �֐��̂��̂Ɠ���
	 */
	void(*TransformPremultiply)(float* dst, const float* src);

	/**
	* @brief �z�� dst ���s��ƌ��Ȃ��ċt�s������߂�B���ʂ�dst�Ɋi�[�����B
	* @param dst 6�v�f�ȏ�̔z��B
	* @param src 6�v�f�ȏ�̔z��B
	* @retval �t�s�񂪑��݂���ꍇ�́`���Ȃ��ꍇ�́`�B
	*/
	int(*TransformInverse)(float* dst, const float* src);

	/**
	 * @brief �z�� xform ���s��ƌ��Ȃ��ē_(srcx, srcy)�ɕό`�s�� xform ��K�p�������ʓ�����_�̍��W��(dstx, dsty)�Ɋi�[����B
	 * @param dstx �ό`��̓_��X���W���i�[����ϐ��̃|�C���^
	 * @param dsty �ό`��̓_��Y���W���i�[����ϐ��̃|�C���^
	 * @param xform �ό`�ɗp����s��Ƃ��Ă݂Ȃ�6�v�f�ȏ�̔z��
	 * @param srcx �ό`�O�̓_��X���W
	 * @param srcy �ό`�O�̓_��Y���W
	 */
	void(*TransformPoint)(float* dstx, float* dsty, const float* xform, float srcx, float srcy);

	/**
	* @brief �p�x�����W�A���ɕϊ�
	* @param deg �p�x
	* @return deg�ɑΉ����郉�W�A��
	*/

	float(*DegToRad)(float deg);

	/**
	* @brief ���W�A�����p�x�ɕϊ�
	* @param rad ���W�A��
	* @return rad�ɑΉ�����p�x
	*/
	float(*RadToDeg)(float rad);

	/**
	 * @brief �摜�f�[�^���t�@�C������ǂݍ��ށi�Ή�����`����JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC�j
	 * @param filename �摜�t�@�C���̃t�@�C���p�X
	 * @param imageFlags �摜��`��ŗp����ۂ̃I�v�V������ enum ImageFlags �̃[���ȏ�̑g�ݍ��킹�Ŏw��
	 * @retval  == 0 �ǂݍ��ݎ��s
	 * @retval   > 0 �摜ID
	 */
	int(*CreateImage)(const char* filename, enum ImageFlags imageFlags);

	/**
	 * @brief �摜�f�[�^���������ォ��ǂݍ��ށi�Ή�����`����JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC�j
	 * @param imageFlags �摜��`��ŗp����ۂ̃I�v�V������ enum ImageFlags �̃[���ȏ�̑g�ݍ��킹�Ŏw��
	 * @param data �摜�f�[�^�̃|�C���^
	 * @param ndata �摜�f�[�^�̃T�C�Y
	 * @retval  == 0 �ǂݍ��ݎ��s
	 * @retval   > 0 �摜ID
	 */
	int(*CreateImageMem)(enum ImageFlags imageFlags, unsigned char* data, int ndata);


	/**
	 * @brief 32bitRGBA�摜�f�[�^��ǂݍ���
	 * @param w �摜�̕�
	 * @param h �摜�̍���
	 * @param imageFlags �摜��`��ŗp����ۂ̃I�v�V������ enum ImageFlags �̃[���ȏ�̑g�ݍ��킹�Ŏw��
	 * @param data �摜�f�[�^�̃|�C���^
	 * @retval  == 0 �ǂݍ��ݎ��s
	 * @retval   > 0 �摜ID
	 */
	int(*CreateImageRGBA)(int w, int h, enum ImageFlags imageFlags, const unsigned char* data);

	/**
	* @brief 32bitRGBA�摜�f�[�^ data �ŉ摜ID img �̉摜�f�[�^��u��������B�摜�T�C�Y�Ȃǂ͕ύX�ł��Ȃ�
	* @param image �摜ID
	* @param data �摜�f�[�^�̃|�C���^
	* @note �����E�F�u�J�����̉摜�ȂǃT�C�Y�͌Œ肾�����Ԃŕω�����摜��`�悷��ꍇ�ɗp����
	*/
	void(*UpdateImage)(int image, const unsigned char* data);

	/**
	* @brief �摜�T�C�Y���擾
	* @param image �摜ID
	* @param w �摜�̕��̊i�[��|�C���^
	* @param h �摜�̍����̊i�[��|�C���^
	*/
	void(*ImageSize)(int image, int* w, int* h);

	/**
	 * @brief �摜��j������
	 * @param image �摜ID
	 */
	void(*DeleteImage)(int image);

	/**
	 * @brief ���`�O���f�[�V�����̓h��Ԃ��p�^�[�����쐬
	 * @param sx �O���f�[�V�����̎n�_X���W
	 * @param sy �O���f�[�V�����̎n�_Y���W
	 * @param ex �O���f�[�V�����̏I�_X���W
	 * @param ey �O���f�[�V�����̏I�_Y���W
	 * @param icol �n�_���̐F
	 * @param ocol �I�_���̐F
	 * @return �쐬���ꂽ�h��Ԃ��p�^�[��
	 */
	Paint(*LinearGradient)(float sx, float sy, float ex, float ey, Color icol, Color ocol);

	/**
	 * @brief ��`�O���f�[�V�����̓h��Ԃ��p�^�[�����쐬
	 * @param x ��`�̍���_��X���W
	 * @param y ��`�̍���_��Y���W
	 * @param w ��`�̕�
	 * @param h ��`�̍���
	 * @param r ��`�̊p�̊ۂߔ��a
	 * @param f ��`�̋��E�̂ڂ�������
	 * @param icol �n�_�i�����j���̐F
	 * @param ocol �I�_�i���E�j���̐F
	 * @return �쐬���ꂽ�h��Ԃ��p�^�[��
	 */
	Paint(*BoxGradient)(float x, float y, float w, float h, float r, float f, Color icol, Color ocol);

	/**
	 * @brief �~�`�O���f�[�V�����̓h��Ԃ��p�^�[�����쐬
	 * @param cx �~�̒��S�_��X���W
	 * @param cy �~�̒��S�_��Y���W
	 * @param inr �����̔��a
	 * @param outr �O���̔��a
	 * @param icol �����̐F
	 * @param ocol �O���̐F
	 * @return �쐬���ꂽ�h��Ԃ��p�^�[��
	 */
	Paint(*RadialGradient)(float cx, float cy, float inr, float outr, Color icol, Color ocol);

	/**
	* @brief �摜�p�^�[�����쐬
	* @param ox �p�^�[���Ɏg���摜�̈�i��`�j�̍���_��X���W
	* @param oy �p�^�[���Ɏg���摜�̈�i��`�j�̍���_��Y���W
	* @param ex �p�^�[���Ɏg���摜�̈�i��`�j�̕�
	* @param ey �p�^�[���Ɏg���摜�̈�i��`�j�̍���
	* @param angle �摜�̉�]�p�x�i��]���S���p�^�[���̍���ł��邱�Ƃɒ��Ӂj
	* @param image �摜ID
	* @param alpha �摜�̕s�����x(0.0�`1.0�Ŏw��)
	* @return �쐬���ꂽ�h��Ԃ��p�^�[��
	*/
	Paint(*ImagePattern)(float ox, float oy, float ex, float ey, float angle, int image, float alpha);

#if defined(UNOFFICIAL_HACK_FILL_COLOR_AFFECTS_THE_IMAGE_PATTERN)
	/**
	* @brief �摜�p�^�[�����쐬(�摜�̐F�Ƃ���FillColor�ɐݒ肳��Ă���F��p����)
	* @param ox �p�^�[���Ɏg���摜�̈�i��`�j�̍���_��X���W
	* @param oy �p�^�[���Ɏg���摜�̈�i��`�j�̍���_��Y���W
	* @param ex �p�^�[���Ɏg���摜�̈�i��`�j�̕�
	* @param ey �p�^�[���Ɏg���摜�̈�i��`�j�̍���
	* @param angle �摜�̉�]�p�x�i��]���S���p�^�[���̍���ł��邱�Ƃɒ��Ӂj
	* @param image �摜ID
	* @return �쐬���ꂽ�h��Ԃ��p�^�[��
	* ���̖��߂͓���Ȏg������z�肵�Ă���i�������o�̃p�[�e�B�N���Ƀe�N�X�`�����g���ĐԐF��ǉ�����Ȃǁj�B
	* ���߂̍\���ɂ͂��̐�������������\��Ȃ̂ŕK�v�łȂ�����g�p������Ă��������B
	*/
	Paint(*ImagePatternWithFillColor)(float cx, float cy, float w, float h, float angle, int image);
#endif

	/**
	* @brief �V�U�����O�i�N���b�s���O�j�̈��V�K�ݒ肷��
	* @param x �V�U�����O�i�N���b�s���O�j�̈�i��`�j�̍���_��X���W
	* @param y �V�U�����O�i�N���b�s���O�j�̈�i��`�j�̍���_��Y���W
	* @param w �V�U�����O�i�N���b�s���O�j�̈�i��`�j�̕�
	* @param h �V�U�����O�i�N���b�s���O�j�̈�i��`�j�̍���
	* �V�U�����O�i�N���b�s���O�j�̈��ݒ肷��Ƃ��̗̈�̊O���ɂ͕`�悪�s���Ȃ��Ȃ�B
	* ��ʕ����Ńv���C���[���Ƃɉ�ʂ�`�悷��ۂɗp����Ɩ��ʂȕ`������炷���Ƃ��ł���B
	*/
	void(*Scissor)(float x, float y, float w, float h);

	/**
	* @brief �V�U�����O�i�N���b�s���O�j�̈��ݒ肳��Ă���̈�ƈ����ŗ^�����̈�̋��ʗ̈�ɍX�V����
	* @param x �d�˂�V�U�����O�i�N���b�s���O�j�̈�i��`�j�̍���_��X���W
	* @param y �d�˂�V�U�����O�i�N���b�s���O�j�̈�i��`�j�̍���_��Y���W
	* @param w �d�˂�V�U�����O�i�N���b�s���O�j�̈�i��`�j�̕�
	* @param h �d�˂�V�U�����O�i�N���b�s���O�j�̈�i��`�j�̍���
	*/
	void(*IntersectScissor)(float x, float y, float w, float h);

	/**
	* @brief �V�U�����O�i�N���b�s���O�j�̈�����Z�b�g����B���Z�b�g��͉�ʑS�̂��V�����V�U�����O�̈�ɂȂ�
	*/
	void(*ResetScissor)(void);

	/**
	 * @brief �}�`�̕`����J�n����
	 * �`�抮����͕K��Video.EndFrame()���Ăяo�����ƁB
	 */
	void(*BeginPath)(void);

	/**
	 * @brief �}�`��`�悷��y���̍��W��ݒ肷��
	 * @param x �y����X���W
	 * @param y �y����Y���W
	 */
	void(*MoveTo)(float x, float y);

	/**
	 * @brief ���݂̃y�����W�Ǝw�肵�����W�̊Ԃɕӂ��쐬���A�y�����ړ�������
	 * @param x �ړ���̃y����X���W
	 * @param y �ړ���̃y����Y���W
	 */
	void(*LineTo)(float x, float y);

	/**
	 * @brief ���݂̃y�����W�A����_1�A����_2�A�w�肵�����W��p����3���x�W�F�Ȑ� (Cubic Bezier curve) ���쐬���A�y�����ړ�������
	 * @param c1x ����_1��X���W
	 * @param c1y ����_1��Y���W
	 * @param c2x ����_2��X���W
	 * @param c2y ����_2��y���W
	 * @param x �ړ���̃y����X���W
	 * @param y �ړ���̃y����Y���W
	 * �x�W�F�Ȑ��̊J�݂� https://ja.wikipedia.org/wiki/%E3%83%99%E3%82%B8%E3%82%A7%E6%9B%B2%E7%B7%9A �Ȃǂ��Q��
	 */
	void(*BezierTo)(float c1x, float c1y, float c2x, float c2y, float x, float y);

	/**
	* @brief ���݂̃y�����W�A����_�A�w�肵�����W��p����2���x�W�F�Ȑ� (Quadratic Bezier curve)���쐬���A�y�����ړ�������
	* @param cx ����_��X���W
	* @param cy ����_��Y���W
	* @param x �ړ���̃y����X���W
	* @param y �ړ���̃y����Y���W
	* �x�W�F�Ȑ��̊J�݂� https://ja.wikipedia.org/wiki/%E3%83%99%E3%82%B8%E3%82%A7%E6%9B%B2%E7%B7%9A �Ȃǂ��Q��
	*/
	void(*QuadTo)(float cx, float cy, float x, float y);

	/**
	* @brief ���݂̃y���ʒu�Ɛ���_�ƈړ���̓_�����ɔ��aradius�̉~�ʂ�`��
	* @param x1 ����_��X���W
	* @param y1 ����_��Y���W
	* @param x2 �ړ���̃y����X���W
	* @param y2 �ړ���̃y����Y���W
	* @param radius �~�ʂ̔��a
	* �~�ʕ`��̉���� http://www.htmq.com/canvas/arcTo.shtml ���܂Ƃ܂��Ă���B
	*/
	void(*ArcTo)(float x1, float y1, float x2, float y2, float radius);

	/**
	* @brief �}�`�̕`�������������
	*/
	void(*ClosePath)(void);

	/**
	 * @brief ���O�ɒǉ������}�`�̌`��̓�������or�ʂŎw�肷��
	 * @param dir ���������� enum Solidity �l
	 */
	void(*PathWinding)(enum Solidity dir);

	/**
	 * @brief ���S�_ (cx, cy) �Ŕ��a r �̉~�̉~����Ŏ��v���Ŋp�x a0 ���� a1 �܂ł̉~�ʂ�`��
	 * @param cx �~�̒��S�_��X���W
	 * @param cy �~�̒��S�_��Y���W
	 * @param r  �~�̔��a
	 * @param a1 �~�ʂ̎n�_�p�x
	 * @param a2 �~�ʂ̏I�_�p�x
	 * @param dir �n�_�ƏI�_�ƌ��ԍۂ̉�]���������� enum Winding �l
	 */
	void(*Arc)(float cx, float cy, float r, float a0, float a1, enum Winding dir);

	/**
	 * @brief ������W(x,y) �� w ���� h �̋�`��`�悷��
	 * @param x ��`�̍���_��X���W
	 * @param y ��`�̍���_��Y���W
	 * @param w ��`�̕�
	 * @param h ��`�̍���
	 */
	void(*Rect)(float x, float y, float w, float h);

	/**
	 * @brief ������W(x,y) �� w ���� h �ph�𔼌a r �Ŋۂ߂���`��`�悷��
	 * @param x ��`�̍���_��X���W
	 * @param y ��`�̍���_��Y���W
	 * @param w ��`�̕�
	 * @param h ��`�̍���
	 * @param r ��`�̊p�̊ۂߔ��a
	 */
	void(*RoundedRect)(float x, float y, float w, float h, float r);

	/**
	 * @brief ���S�_ (cx, cy) �ŉ����a rx �c���a ry �̑ȉ~��`��
	 * @param cx �ȉ~�̒��S�_��X���W
	 * @param cy �ȉ~�̒��S�_��Y���W
	 * @param rx �ȉ~�̉����a
	 * @param ry �ȉ~�̏c���a
	 */
	void(*Ellipse)(float cx, float cy, float rx, float ry);

	/**
	 * @brief ���S�_ (cx, cy) �Ŕ��a r �̉~��`��
	 * @param cx �~�̒��S�_��X���W
	 * @param cy �~�̒��S�_��Y���W
	 * @param r �~�̔��a
	 */
	void(*Circle)(float cx, float cy, float r);

	/**
	* @brief �`�����`���h��Ԃ�
	* �h��Ԃ��� Video.FillColor() �Őݒ肵���F�A�������� Video.FillPaint() �Őݒ肵���p�^�[�����g����
	*/
	void(*Fill)(void);

	/**
	 * @brief �`�����`��̗֊s����`�悷��
	 * ���� Video.StrokeColor() �Őݒ肵���F�A�������� Video.StrokePaint() �Őݒ肵���p�^�[����p���� Video.StrokeWidth()�Őݒ肳�ꂽ�����ŕ`�悳���
	 */
	void(*Stroke)(void);

	/**
	 * @brief TrueType�`���̃t�H���g��TTF�t�@�C�� filename ����ǂݍ��݁A�t�H���g���ɖ��O name �����蓖�Ă�
	 * @param name �ǂݍ��񂾃t�H���g�Ɋ��蓖�Ă閼�O
	 * @param filename �ǂݍ���TTF�`���̃t�H���g�t�@�C��
	 * @retval == -1 �ǂݍ��ݎ��s
	 * @retval >=  0 �ǂݍ��񂾃t�H���g�Ɋ��蓖�Ă���t�H���gID
	 */
	int(*CreateFont)(const char* name, const char* filename);

	/**
	 * @brief TrueType�`���̃t�H���g�f�[�^ data ������������ǂݍ��݁A�t�H���g���ɖ��O name �����蓖�Ă�
	 * @param name �ǂݍ��񂾃t�H���g�Ɋ��蓖�Ă閼�O
	 * @param data �ǂݍ���TTF�`���̃t�H���g�f�[�^
	 * @param ndata �ǂݍ���TTF�`���̃t�H���g�f�[�^�̃T�C�Y
	 * @param freeData �ʏ펞�͎g��Ȃ������Ȃ̂� 0 ��ݒ肷�邱��
	 * @retval == -1 �ǂݍ��ݎ��s
	 * @retval >=  0 �ǂݍ��񂾃t�H���g�Ɋ��蓖�Ă���t�H���gID
	 */
	int(*CreateFontMem)(const char* name, unsigned char* data, int ndata, int freeData);

	/**
	 * @brief ���ɓǂݍ��񂾃t�H���g�̒����疼�O�� name �̂��̂�T��
	 * @param name �T���t�H���g�̖��O
	 * @retval == -1 ������Ȃ�����
	 * @retval >=  0 ���������t�H���g�Ɋ��蓖�Ă��Ă���t�H���gID
	 */
	int(*FindFont)(const char* name);

	/**
	 * @brief �`�悷�镶���̃T�C�Y��ݒ肷��
	 * @param size �t�H���g�T�C�Y
	 */
	void(*FontSize)(float size);

	/**
	* @brief �`�悷�镶���̂ڂ���������ݒ肷��
	* @param size ���E���ڂ�������
	*/
	void(*FontBlur)(float blur);

	/**
	* @brief �`�悷�镶���̕����Ԋu��ݒ肷��
	* @param size �����Ԋu
	*/
	void(*TextLetterSpacing)(float spacing);

	/**
	* @brief �`�悷�镶���̍s����ݒ肷��
	* @param size �s��
	*/
	void(*TextLineHeight)(float lineHeight);

	/**
	* @brief �`�悷�镶���̔z�u���w�肷��
	* @param align �c�����ꂼ��� enum Align �̑g�ݍ��킹�Ŏw�肷��
	*/
	void(*TextAlign)(enum Align align);

	/**
	* @brief �����̕`��Ɏg���t�H���g���t�H���gID�Ŏw�肷��
	* @param font �`��Ɏg���t�H���g�̃t�H���gID
	*/
	void(*FontFaceId)(int font);

	/**
	* @brief �����̕`��Ɏg���t�H���g���t�H���g���Ŏw�肷��
	* @param font �`��Ɏg���t�H���g�̃t�H���g��
	*/
	void(*FontFace)(const char* font);

	/**
	* @brief Video.Text()�֐���UTF8��p��
	*/
	float(*TextUTF8)(float x, float y, const char* string, const char* end);

	/**
	* @brief ���W (x, y)����_�Ƃ��ĕ����� string ��`�悷��
	* @param x ��_��X���W
	* @param y ��_��Y���W
	* @param string �`�悷�镶����̎n�_�������|�C���^
	* @param end    �`�悷�镶����̏I�_�������|�C���^�B�^���������񂷂ׂĂ�`�悵�����ꍇ�� NULL ��n���B
	* @return �Ō�ɕ`����������X���W
	*/
	float(*Text)(float x, float y, const char* string, const char* end);

	/**
	* @brief Video.TextBox()�֐���UTF8��p��
	*/
	void(*TextBoxUTF8)(float x, float y, float breakRowWidth, const char* string, const char* end);

	/**
	* @brief �e�L�X�g��܂�Ԃ����� breakRowWidth �A���W (x, y)����_�Ƃ��ĕ����� string ��`�悷��
	* @param x ��_��X���W
	* @param y ��_��Y���W
	* @param breakRowWidth �e�L�X�g��܂�Ԃ���
	* @param string �`�悷�镶����̎n�_�������|�C���^
	* @param end    �`�悷�镶����̏I�_�������|�C���^�B�^���������񂷂ׂĂ�`�悵�����ꍇ�� NULL ��n���B
	*/
	void(*TextBox)(float x, float y, float breakRowWidth, const char* string, const char* end);

	/**
	* @brief Video.TextBounds()�֐���UTF8��p��
	*/
	float(*TextBoundsUTF8)(float x, float y, const char* string, const char* end, float* bounds);

	/**
	* @brief ���W (x, y)����_�Ƃ��ĕ����� string ��`�悵���Ƃ��̕`��̈���擾����i�`��͍s��Ȃ��j
	* @param x ��_��X���W
	* @param y ��_��Y���W
	* @param string �`�悷�镶����̎n�_�������|�C���^
	* @param end    �`�悷�镶����̏I�_�������|�C���^�B�^���������񂷂ׂĂ�`�悵�����ꍇ�� NULL ��n���B
	* @param bounds �`�悷�镶���񂪂҂��������̈�̏�񂪊i�[�����S�v�f�ȏ�̔z��B[�����X���W, �����Y���W, �E���X���W, �E���Y���W] �̏��Ŋi�[�����
	* @return �Ō�ɕ`����������X���W
	*/
	float(*TextBounds)(float x, float y, const char* string, const char* end, float* bounds);

	/**
	* @brief Video.TextBoxBounds()�֐���UTF8��p��
	*/
	void(*TextBoxBoundsUTF8)(float x, float y, float breakRowWidth, const char* string, const char* end, float* bounds);

	/**
	* @brief ���W (x, y)����_�Ƃ��A�e�L�X�g��܂�Ԃ����� breakRowWidth�Ƃ��ĕ����� string ��`�悵���Ƃ��̕`��̈���擾����i�`��͍s��Ȃ��j
	* @param x ��_��X���W
	* @param y ��_��Y���W
	* @param breakRowWidth �e�L�X�g��܂�Ԃ���
	* @param string �`�悷�镶����̎n�_�������|�C���^
	* @param end    �`�悷�镶����̏I�_�������|�C���^�B�^���������񂷂ׂĂ�`�悵�����ꍇ�� NULL ��n���B
	* @param bounds �`�悷�镶���񂪂҂��������̈�̏�񂪊i�[�����S�v�f�ȏ�̔z��B[�����X���W, �����Y���W, �E���X���W, �E���Y���W] �̏��Ŋi�[�����
	*/
	void(*TextBoxBounds)(float x, float y, float breakRowWidth, const char* string, const char* end, float* bounds);

	/**
	* @brief Video.TextGlyphPositions()�֐���UTF8��p��
	*/
	int(*TextGlyphPositionsUTF8)(float x, float y, const char* string, const char* end, GlyphPosition* positions, int maxPositions);

	/**
	* @brief ���W (x, y)����_�Ƃ��ĕ����� string ��`�悵���Ƃ��̊e�����̈ʒu���� maxPositions �擾���� �z�� positions �Ɋi�[����B
	* @param x ��_��X���W
	* @param y ��_��Y���W
	* @param string �`�悷�镶����̎n�_�������|�C���^
	* @param end    �`�悷�镶����̏I�_�������|�C���^�B�^���������񂷂ׂĂ�`�悵�����ꍇ�� NULL ��n���B
	* @param positions �e�����̈ʒu�����i�[���� GlyphPosition �^�ϐ��z��B�v�f����maxPositions�ȏ�łȂ���΂Ȃ�Ȃ��B
	* @param maxPositions �擾�����񐔂̏��
	* @return ���ۂ�positions�Ɋi�[���ꂽ�v�f�̐�
	* �Q�[�����Ƀe�L�X�g���͗p��GUI�����ꍇ�ȂǂŎg��
	*/
	int(*TextGlyphPositions)(float x, float y, const char* string, const char* end, GlyphPosition* positions, int maxPositions);

	/**
	* @brief �`��ɗp����t�H���g�̃��g���N�X���(�A�Z���_�A�f�B�Z���_�A�s����)���擾����
	* @param ascender �t�H���g�̃A�Z���_�i�x�[�X���C�����當���㕔�܂ł̍����j
	* @param descender �t�H���g�̃f�B�Z���_�i�x�[�X���C�����當�������܂ł̍����j
	* @param lineh �t�H���g�̍s����
	*/
	void(*TextMetrics)(float* ascender, float* descender, float* lineh);

	/**
	* @brief Video.TextBreakLines()�֐���UTF8��p��
	*/
	int(*TextBreakLinesUTF8)(const char* string, const char* end, float breakRowWidth, TextRow* rows, int maxRows);

	/**
	* @brief �e�L�X�g��܂�Ԃ����� breakRowWidth�Ƃ��ĕ����� string ��`�悷��ۂ̉�ʏ�ł̊e�s�̏��𓾂�
	* @param string �`�悷�镶����̎n�_�������|�C���^
	* @param end    �`�悷�镶����̏I�_�������|�C���^�B�^���������񂷂ׂĂ�`�悵�����ꍇ�� NULL ��n���B
	* @param positions �e�s�̏����i�[���� TextRow �^�ϐ��z��B�v�f����maxRows�ȏ�łȂ���΂Ȃ�Ȃ��B
	* @param maxPositions �擾�����񐔂̏��
	* @return ���ۂ�positions�Ɋi�[���ꂽ�v�f�̐�
	* �Q�[�����Ƀe�L�X�g���͗p��GUI��N���b�N�\�ȃe�L�X�g�E�B���h�E�����ꍇ�ȂǂŎg��
	*/
	int(*TextBreakLines)(const char* string, const char* end, float breakRowWidth, TextRow* rows, int maxRows);

	/**
	* @brief �摜�̍�����W(sx, sy), ��sw, ����sh �Ŏ������̈����ʂ̍�����W(dx,dy), �� dw, ���� dh �Ɋp�x angle, �s�����x alhpa �ŕ`�悷��
	* @param image �摜ID
	* @param sx �]�����̈�̍����X���W
	* @param sy �]�����̈�̍����Y���W
	* @param sw �]�����̈�̕�
	* @param sh �]�����̈�̍���
	* @param dx �]����̈�̍����X���W
	* @param dy �]����̈�̍����Y���W
	* @param dw �]����̈�̕�
	* @param dh �]����̈�̍���
	* @param angle �摜�̉�]�p�x�i��]���S�͉摜�̍���ł��邱�Ƃɒ��Ӂj
	* @param alpha �s�����x(1.0�����S�s����, 0.0�����S����)
	*/
	void(*DrawImage)(int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle, float alpha);

	/**
	* @brief �摜�̍�����W(sx, sy), ��sw, ����sh �Ŏ������̈����ʂ̍�����W(dx,dy), �� dw, ���� dh �Ɋp�x angle�ŕ`�悷��B�`��F�ɂ�FillColor��p����B
	* @param image �摜ID
	* @param sx �]�����̈�̍����X���W
	* @param sy �]�����̈�̍����Y���W
	* @param sw �]�����̈�̕�
	* @param sh �]�����̈�̍���
	* @param dx �]����̈�̍����X���W
	* @param dy �]����̈�̍����Y���W
	* @param dw �]����̈�̕�
	* @param dh �]����̈�̍���
	* @param angle �摜�̉�]�p�x�i��]���S�͉摜�̍���ł��邱�Ƃɒ��Ӂj
	*/
	void(*DrawImageWithFillColor)(int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle);

	/**
	* @brief ���W (x, y)����_�Ƃ��Aprintf���l�� str ��������������A���̌�ɑ����������p�����[�^�Ƃ��ėp���ĕ�������\�z���ĕ`�悷��
	* @param x ��_��X���W
	* @param y ��_��Y���W
	* @param str ������������ (printf�̂��̂Ɠ���)
	* @param ... �p�����[�^ (printf�̂��̂Ɠ���)
	* �X�R�A�␧�����Ԃ̕`��Ȃǎg����B
	*/
	void(*FormatText)(float x, float y, const char *str, ... );

} Video;

/**
 * @brief �}�E�X���
 */
extern const struct __tagMouseAPI {
	/**
	 * @brief �}�E�X�̍��{�^����������Ă��邩�ǂ������ׂ�
	 * @retval true ������Ă���
	 * @retval false ������Ă��Ȃ�
	 */
	bool(*IsLeftButtonDown)(void);

	/**
	* @brief �}�E�X�̉E�{�^����������Ă��邩�ǂ������ׂ�
	* @retval true ������Ă���
	* @retval false ������Ă��Ȃ�
	*/
	bool(*IsRightButtonDown)(void);

	/**
	* @brief �}�E�X�̃z�C�[����������Ă��邩�ǂ������ׂ�
	* @retval true ������Ă���
	* @retval false ������Ă��Ȃ�
	*/
	bool(*IsMiddleButtonDown)(void);

	/**
	* @brief ��ʏ�ɂ�����J�[�\���̈ʒu�𓾂�
	* @param x �J�[�\����x���W���i�[�����ϐ��̃|�C���^
	* @param y �J�[�\����y���W���i�[�����ϐ��̃|�C���^
	*/
	void(*GetCursorPos)(double *x, double *y);
} Mouse;
/*
 * �L�[�{�[�h�̃L�[�������R�[�h
 */
enum Keycode
{
	KBKEY_SPACE = 32,
	KBKEY_APOSTROPHE = 39,  /* ' */
	KBKEY_COMMA = 44,  /* , */
	KBKEY_MINUS = 45,  /* - */
	KBKEY_PERIOD = 46,  /* . */
	KBKEY_SLASH = 47,  /* / */
	KBKEY_0 = 48,
	KBKEY_1 = 49,
	KBKEY_2 = 50,
	KBKEY_3 = 51,
	KBKEY_4 = 52,
	KBKEY_5 = 53,
	KBKEY_6 = 54,
	KBKEY_7 = 55,
	KBKEY_8 = 56,
	KBKEY_9 = 57,
	KBKEY_SEMICOLON = 59,  /* ; */
	KBKEY_EQUAL = 61,  /* = */
	KBKEY_A = 65,
	KBKEY_B = 66,
	KBKEY_C = 67,
	KBKEY_D = 68,
	KBKEY_E = 69,
	KBKEY_F = 70,
	KBKEY_G = 71,
	KBKEY_H = 72,
	KBKEY_I = 73,
	KBKEY_J = 74,
	KBKEY_K = 75,
	KBKEY_L = 76,
	KBKEY_M = 77,
	KBKEY_N = 78,
	KBKEY_O = 79,
	KBKEY_P = 80,
	KBKEY_Q = 81,
	KBKEY_R = 82,
	KBKEY_S = 83,
	KBKEY_T = 84,
	KBKEY_U = 85,
	KBKEY_V = 86,
	KBKEY_W = 87,
	KBKEY_X = 88,
	KBKEY_Y = 89,
	KBKEY_Z = 90,
	KBKEY_LEFT_BRACKET = 91,  /* [ */
	KBKEY_BACKSLASH = 92,  /* \ */
	KBKEY_RIGHT_BRACKET = 93,  /* ] */
	KBKEY_GRAVE_ACCENT = 96,  /* ` */
	KBKEY_WORLD_1 = 161, /* non-US #1 */
	KBKEY_WORLD_2 = 162, /* non-US #2 */

	/* Function keys */
	KBKEY_ESCAPE = 256,
	KBKEY_ENTER = 257,
	KBKEY_TAB = 258,
	KBKEY_BACKSPACE = 259,
	KBKEY_INSERT = 260,
	KBKEY_DELETE = 261,
	KBKEY_RIGHT = 262,
	KBKEY_LEFT = 263,
	KBKEY_DOWN = 264,
	KBKEY_UP = 265,
	KBKEY_PAGE_UP = 266,
	KBKEY_PAGE_DOWN = 267,
	KBKEY_HOME = 268,
	KBKEY_END = 269,
	KBKEY_CAPS_LOCK = 280,
	KBKEY_SCROLL_LOCK = 281,
	KBKEY_NUM_LOCK = 282,
	KBKEY_PRINT_SCREEN = 283,
	KBKEY_PAUSE = 284,
	KBKEY_F1 = 290,
	KBKEY_F2 = 291,
	KBKEY_F3 = 292,
	KBKEY_F4 = 293,
	KBKEY_F5 = 294,
	KBKEY_F6 = 295,
	KBKEY_F7 = 296,
	KBKEY_F8 = 297,
	KBKEY_F9 = 298,
	KBKEY_F10 = 299,
	KBKEY_F11 = 300,
	KBKEY_F12 = 301,
	KBKEY_F13 = 302,
	KBKEY_F14 = 303,
	KBKEY_F15 = 304,
	KBKEY_F16 = 305,
	KBKEY_F17 = 306,
	KBKEY_F18 = 307,
	KBKEY_F19 = 308,
	KBKEY_F20 = 309,
	KBKEY_F21 = 310,
	KBKEY_F22 = 311,
	KBKEY_F23 = 312,
	KBKEY_F24 = 313,
	KBKEY_F25 = 314,
	KBKEY_KP_0 = 320,
	KBKEY_KP_1 = 321,
	KBKEY_KP_2 = 322,
	KBKEY_KP_3 = 323,
	KBKEY_KP_4 = 324,
	KBKEY_KP_5 = 325,
	KBKEY_KP_6 = 326,
	KBKEY_KP_7 = 327,
	KBKEY_KP_8 = 328,
	KBKEY_KP_9 = 329,
	KBKEY_KP_DECIMAL = 330,
	KBKEY_KP_DIVIDE = 331,
	KBKEY_KP_MULTIPLY = 332,
	KBKEY_KP_SUBTRACT = 333,
	KBKEY_KP_ADD = 334,
	KBKEY_KP_ENTER = 335,
	KBKEY_KP_EQUAL = 336,
	KBKEY_LEFT_SHIFT = 340,
	KBKEY_LEFT_CONTROL = 341,
	KBKEY_LEFT_ALT = 342,
	KBKEY_LEFT_SUPER = 343,
	KBKEY_RIGHT_SHIFT = 344,
	KBKEY_RIGHT_CONTROL = 345,
	KBKEY_RIGHT_ALT = 346,
	KBKEY_RIGHT_SUPER = 347,
	KBKEY_MENU = 348,
};

/**
* @brief �L�[�{�[�h���
*/
extern const struct __tagKeyboardAPI {
	/**
	* @brief �L�[�{�[�h�̎w�肵���L�[��������Ă��邩���ׂ�
	* @param key ���ׂ����L�[������ enum Keycode �̒l
	* @retval true ������Ă���
	* @retval false ������Ă��Ȃ�
	* �X�y�[�X�A�A�|�X�g���t�B�A�R���}�A�}�C�i�X�A�s���I�h�A�X���b�V���A�Z�~�R�����A�C�R�[���A
	* ������1�`10�A�啶����A�`Z�A�u���P�b�g�L���̃L�[�ɂ��Ă� 'A'�̂悤�ȕ\�L�ł��悢
	*/
	bool(*IsKeyDown)(enum Keycode key);
} Keyboard;

/**
 * @brief ��ʂ̍X�V�Ԋu�i�t���[�����[�g�j���
 */
extern const struct __tagFrameRateAPI {
	void(*SetFrameRate)(unsigned int fps);
	unsigned int(*GetFrameRate)(void);
	float(*GetRealFrameRate)();
	unsigned int(*GetCPUPower)(void);
} FrameRate;

