require "fox16"
require 'rmagick'
include Math
include Magick
include Fox
require 'tenderjit'

class Photo
  
  attr_reader :path
  
  def initialize(path)
    @path  = path
  end

end

class PhotoView < FXImageFrame

  def initialize(p, photo)
      super(p, nil)
      load_image(photo.path)
  end

  def load_image(path)
    File.open(path, "rb" ) do |io|
      self.image = FXJPGImage.new(app, io.read)
    end
  end

end

class AlbumView < FXMatrix  

  attr_reader :album

  def initialize(p, album)
    super(p, :opts => LAYOUT_FILL)
    @album = album
    @album.each { |photo| add_photo(photo) }
  end

  def add_photo(photo)
    PhotoView.new(self, photo)
  end


end

class ImageWindow < FXMainWindow

  def initialize(app)
    # Invoke base class initializer first
    super(app, 'Размытие Гаусса', opts: DECOR_ALL, width: 1360, height: 760)
    photo = ImageList.new("img/grushi.jpg")
    jit = TenderJIT.new
    photo.resize_to_fill(443,600).write("img/hb_resize.jpg")
    photo = photo.channel(GrayChannel)
    photo = gauss_init(photo)
    photo.write("img/hb_gauss_ruby.jpg")
    photo.resize_to_fill(443,600).write("img/hb_gauss_ruby_resize.jpg")
    canny_img = canny(photo, 3)
    canny_img.display
    canny_img.write("img/canny_1_ruby.jpg")
    canny_img.resize_to_fill(443,600).write("img/canny_1_ruby_resize.jpg")
    @album_view = AlbumView.new(self, [Photo.new("img/hb_resize.jpg"), Photo.new("img/hb_gauss_ruby_resize.jpg"), Photo.new("img/canny_1_ruby_resize.jpg")])
    label1 = FXLabel.new(self, "Оригинал.", :height => 40, :width => 160, :opts => LAYOUT_FIX_HEIGHT | LAYOUT_FIX_WIDTH|LAYOUT_FIX_X|LAYOUT_FIX_Y,
                           :x => 150, :y => 620)
    label2 = FXLabel.new(self, "С размытием.", :height => 40, :width => 160, :opts => LAYOUT_FIX_HEIGHT | LAYOUT_FIX_WIDTH|LAYOUT_FIX_X|LAYOUT_FIX_Y,
                           :x => label1.getX+300+160, :y => 620)
    label3 = FXLabel.new(self, "Алгоритм Канни.", :height => 40, :width => 160, :opts => LAYOUT_FIX_HEIGHT | LAYOUT_FIX_WIDTH|LAYOUT_FIX_X|LAYOUT_FIX_Y,
                           :x => label2.getX+300+160, :y => 620)
    [label1, label2, label3].each{|i| i.setFont(FXFont.new(app, "Times,125,bold")); i.backColor = 'blue'; i.textColor = "white"}
  end

  def create
    super
    # Make the main window appear
    show(PLACEMENT_SCREEN)
  end

  def img_size img
    [img.columns,img.rows]
  end

  def gauss_init img
    n = 11
    begn = n / 2
    size_img = img_size(img)
    h, w = size_img[0],size_img[1]
    end_h = h - begn 
    end_w = w - begn
    matr_gauss = (0..n-1).map{|i|[]}
    sum_matr = 0 
    (0..n-1).each{|i| (0..n-1).each{|j| matr_gauss[i][j] = gauss(i,j); sum_matr += matr_gauss[i][j]}}
    (0..n-1).each{|i| (0..n-1).each{|j| matr_gauss[i][j] /= sum_matr}}
    gauss_blur = lambda {|ker, foto_grey|(begn..end_h).each{|i| (begn..end_w).each{|j| sum_value = (0..n-1).map{ |k| (0..n-1).map{ |l| ker[k][l]*foto_grey.pixel_color(i+k,j+l).red
      }.sum}.sum;foto_grey.pixel_color(i+2,j+2, Pixel.new(sum_value.to_i,sum_value.to_i,sum_value.to_i,65535))}}; foto_grey}
    gauss_blur.call matr_gauss, img
  end

  def gauss x, y
    a = b = 2
    sig = 1
    (1 / (2 * PI * sig ** 2)) * Math.exp(-((x - a) ** 2 + (y - b) ** 2) / (2 * sig ** 2))
  end

  def doubleFiltr lst_param

    d_matr = lst_param[0]
    size = lst_param[1]
    img = lst_param[2]
    max_grad = lst_param[3]

    low_level = max_grad / 25
    high_level = max_grad / 10
    begn = size / 2
    size_img = img_size(img)
    h, w = size_img[0],size_img[1]
    end_h = h - begn
    end_w = w - begn
    gauss_gray = img.copy()
    for i in (begn..end_h)
        for j in (begn..end_w)
            if (d_matr[i][j]< low_level) & (gauss_gray.pixel_color(i,j).red == 0)
                img.pixel_color(i,j, Pixel.new(65535,65535,65535,65535))
            elsif (d_matr[i][j] < high_level) & ((gauss_gray.pixel_color(i,j).red == 0))
                if (d_matr[i-1][j-1] < high_level) & (d_matr[i][j-1] < high_level) & (d_matr[i+1][j-1] < high_level)
                    if (d_matr[i-1][j] < high_level) & (d_matr[i+1][j] < high_level)
                        if (d_matr[i-1][j+1] < high_level) & (d_matr[i][j+1] < high_level) & (d_matr[i+1][j+1] < high_level)
                            img.pixel_color(i,j, Pixel.new(65535,65535,65535,65535))
            end
                end
                    end
                        end
    end
      end
    img
  end

  def tang x, y, sqr 
    if (x == 0)
        y = 0.001
    end    
    tg = (y+0.0) / x

    if ((x>0 and y<0 and tg<-2.414) or (x<0 and y<0 and tg> 2.414))
       return 0
    elsif (x>0 and y<0 and tg<-0.414)
        return 1
    elsif ((x>0 and y<0 and tg>-0.414) or (x>0 and y>0 and tg< 0.414))
        return 2
    elsif (tg == -0.785)
        return 3
    elsif ((x>0 and y>0 and tg > 2.414) or (x<0 and y>0 and tg< -2.414))
        return 4
    elsif (x < 0 and y > 0 and tg < -0.414)
        return 5
    elsif ((x<0 and y>0 and tg>0.414) or (x<0 and y<0 and tg< 0.414))
        return 6
    elsif (x < 0 and y < 0 and tg < 2.414)
        return 7
    else
        return 0
    end

  end

  def sobel_operation img, size
    sobel_x = [[-1, 0, 1], [-2, 0, 2], [-1, 0, 1]]
    sobel_y = [[-1, -2, -1], [0, 0, 0], [1, 2, 1]]
    begn = size / 2
    size_img = img_size(img)
    h, w = size_img[0],size_img[1]
    end_h = h - begn 
    end_w = w - begn
    gv = (0..h+1).map{|i|[0]* (w+1)}
    gfi = (0..h+1).map{|i|[0]* (w+1)}
    max_grad = 0
    (begn..end_h).each{|i| 
        (begn..end_w).each{|j|
            x = (0..size-1).map{|k| (0..size-1).map{|l| sobel_x[k][l] * img.pixel_color((i - begn + k), (j - begn + l)).red}.sum}.sum;
            y = (0..size-1).map{|x| (0..size-1).map{|n| sobel_y[x][n] * img.pixel_color((i - begn + x), (j - begn + n)).red}.sum}.sum; 
            sqr = Math.sqrt(x * x + y * y);
            if (sqr > max_grad) 
                max_grad = sqr 
            end;   
            gv[i][j] = sqr;   
            gfi[i][j] = tang(x, y, sqr)
    }
        }
    [gv, gfi, max_grad]
  end

  def canny_alg img, size
    gvf_list = sobel_operation img, size

    h, w = img_size(img)
    size_img = img_size(img)
    h, w = size_img[0],size_img[1]
    begn = size / 2 

    end_h = h - begn
    end_w = w - begn
    for i in (begn..end_h)
        for j in (begn..end_w)
            if  gvf_list[1][i][j] == 0 or gvf_list[1][i][j] == 4
                if gvf_list[0][i+1][j] < gvf_list[0][i][j] and gvf_list[0][i][j] > gvf_list[0][i-1][j]
                    img.pixel_color(i, j, Pixel.new(0,0,0,65535))
                else
                    img.pixel_color(i,j, Pixel.new(65535,65535,65535,65535))
              end
            end
            if gvf_list[1][i][j] == 1 or gvf_list[1][i][j] == 5
                if gvf_list[0][i-1][j+1] < gvf_list[0][i][j] and gvf_list[0][i][j] > gvf_list[0][i+1][j-1]
                    img.pixel_color(i, j, Pixel.new(0,0,0,65535))
                else
                    img.pixel_color(i,j, Pixel.new(65535,65535,65535,65535))
              end
            end
            if gvf_list[1][i][j] == 2 or gvf_list[1][i][j] == 6
                if gvf_list[0][i][j+1] < gvf_list[0][i][j] and gvf_list[0][i][j] > gvf_list[0][i][j-1]
                    img.pixel_color(i, j, Pixel.new(0,0,0,65535))
                else
                    img.pixel_color(i,j, Pixel.new(0,0,0,65535))
              end
            end
            if gvf_list[1][i][j] == 3 or gvf_list[1][i][j] == 7
                if gvf_list[0][i-1][j-1] < gvf_list[0][i][j] and gvf_list[0][i][j] > gvf_list[0][i+1][j+1]
                    img.pixel_color(i, j, Pixel.new(0,0,0,65535))
                else
                    img.pixel_color(i,j, Pixel.new(65535,65535,65535,65535))
              end
            end
    end
      end          
    [gvf_list[0], size, img , gvf_list[2]]
  end

  def canny img, size
    lst_param = canny_alg(img, size)
    doubleFiltr(lst_param)
  end
end

if __FILE__ == $0
    FXApp.new do |app|
        ImageWindow.new(app)
        app.create
        app.run
    end
end