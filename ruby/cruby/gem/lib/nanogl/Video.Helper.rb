module NanoGL
  module Video
    class << self

      def Draw(&block)
        if block_given?
          loop do
            self.Drawing()
            yield
          end
        else
          self.Drawing()
        end
      end

      def Path(&block)
        self.BeginPath()
        yield if block_given?
        self.ClosePath()
      end

    end
  end
end
