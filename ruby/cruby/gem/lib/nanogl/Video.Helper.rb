module NanoGL
  module Video
    class << self

      def Draw(&block)
        loop do
          self.Drawing()
          instance_eval(&block) if block_given?
        end
      end

      def Path(&block)
        self.BeginPath()
        instance_eval(&block) if block_given?
        self.ClosePath()
      end

    end
  end
end
