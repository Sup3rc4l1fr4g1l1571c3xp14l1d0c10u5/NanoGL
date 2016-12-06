class PerlinNoise
private	
	#double fade(double t) const noexcept {
	#	return t * t * t * (t * (t * 6 - 15) + 10);
	#}
	def fade(t)
		return t * t * t * (t * (t * 6 - 15) + 10)
	end

	#double lerp(double t, double a, double b) const noexcept {
	#	return a + t * (b - a);
	#}
	def lerp(t,a,b)
		return a + t * (b - a)
	end

	#double grad(std::int32_t hash, double x, double y, double z) const noexcept {
	#	const std::int32_t h = hash & 15;
	#	const double u = h < 8 ? x : y;
	#	const double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	#	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	#}
	def grad(hash, x, y, z)
		h = hash & 15
		u = h < 8 ? x : y
		v = h < 4 ? y : h == 12 || h == 14 ? x : z
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v)
	end

public
	#int32_t p[512];
	#explicit PerlinNoise(std::uint32_t seed = std::default_random_engine::default_seed) {
	#	reseed(seed);
	#}
	def initialize(seed = Random.new_seed)
		reseed(seed)
	end

	#void reseed(std::uint32_t seed) {
	#	for (std::int32_t i = 0; i < 256; ++i) {
	#		p[i] = i;
	#	}

	#	std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

	#	for (size_t i = 0; i < 256; ++i) {
	#		p[256 + i] = p[i];
	#	}
	#}
	def reseed(seed)
		@p = 256.times.to_a.shuffle(random: seed)*2
	end

	#double noise(double x) const
	#{
	#	return noise(x, 0.0, 0.0);
	#}

	#double noise(double x, double y) const
	#{
	#	return noise(x, y, 0.0);
	#}

	#double noise(double x, double y, double z) const
	#{
	#	const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
	#	const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
	#	const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;
	#
	#	x -= std::floor(x);
	#	y -= std::floor(y);
	#	z -= std::floor(z);
	#
	#	const double u = fade(x);
	#	const double v = fade(y);
	#	const double w = fade(z);
	#
	#	const int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
	#	const int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
	#
	#	return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
	#		grad(p[BA], x - 1, y, z)),
	#		lerp(u, grad(p[AB], x, y - 1, z),
	#		grad(p[BB], x - 1, y - 1, z))),
	#		lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
	#		grad(p[BA + 1], x - 1, y, z - 1)),
	#		lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
	#		grad(p[BB + 1], x - 1, y - 1, z - 1))));
	#}

	def noise(x, y=0.0, z=0.0)
		_x = x.floor & 255
		_y = y.floor & 255
		_z = z.floor & 255
	
		x -= x.floor
		y -= y.floor
		z -= z.floor
	
		u = fade(x)
		v = fade(y)
		w = fade(z)
	
		_a  = @p[_x + 0] + _y
		_aa = @p[_a + 0] + _z
		_ab = @p[_a + 1] + _z
		_b  = @p[_x + 1] + _y
		_ba = @p[_b + 0] + _z
		_bb = @p[_b + 1] + _z
	
		return lerp(
				w, 
				lerp(
					v, 
					lerp(
						u, 
						grad(@p[_aa], x    , y, z),
						grad(@p[_ba], x - 1, y, z)
					),
					lerp(
						u, 
						grad(@p[_ab], x    , y - 1, z),
						grad(@p[_bb], x - 1, y - 1, z)
					)
				),
				lerp(
					v, 
					lerp(
						u, 
						grad(@p[_aa + 1], x    , y, z - 1),
						grad(@p[_ba + 1], x - 1, y, z - 1)
					),
					lerp(
						u, 
						grad(@p[_ab + 1], x    , y - 1, z - 1),
						grad(@p[_bb + 1], x - 1, y - 1, z - 1)
					)
				)
			)
	end

	#double octaveNoise(double x, std::int32_t octaves) const
	#{
	#	double result = 0.0;
	#	double amp = 1.0;
	#
	#	for (std::int32_t i = 0; i < octaves; ++i)
	#	{
	#		result += noise(x) * amp;
	#		x *= 2.0;
	#		amp *= 0.5;
	#	}
	#
	#	return result;
	#}

	#double octaveNoise(double x, double y, std::int32_t octaves) const
	#{
	#	double result = 0.0;
	#	double amp = 1.0;
	#
	#	for (std::int32_t i = 0; i < octaves; ++i)
	#	{
	#		result += noise(x, y) * amp;
	#		x *= 2.0;
	#		y *= 2.0;
	#		amp *= 0.5;
	#	}
	#
	#	return result;
	#}

	#double octaveNoise(double x, double y, double z, std::int32_t octaves) const
	#{
	#	double result = 0.0;
	#	double amp = 1.0;
	#
	#	for (std::int32_t i = 0; i < octaves; ++i)
	#	{
	#		result += noise(x, y, z) * amp;
	#		x *= 2.0;
	#		y *= 2.0;
	#		z *= 2.0;
	#		amp *= 0.5;
	#	}
	#
	#	return result;
	#}

	def octaveNoise(octaves, persistence, x, y=0, z=0 )
		result = 0.0
		amp = 1.0
		octaves.times do |i|
			max += amp
			result += noise(x, y, z) * amp
			x *= 2.0
			y *= 2.0
			z *= 2.0
			amp *= persistence
		end
	
		return result
	end

	def octaveNoise0_1(octaves, persistence, x, y=0, z=0 )
		result = 0.0
		amp = 1.0
		max = 0.0
		
		octaves.times do |i|
			max += amp
			result += noise(x, y, z) * amp
			x *= 2.0
			y *= 2.0
			z *= 2.0
			amp *= persistence
		end
	
		return (result / max) * 0.5 + 0.5
	end

	#double noise0_1(double x) const
	#{
	#	return noise(x) * 0.5 + 0.5;
	#}

	def noise0_1(x, y=0, z=0)
		return noise(x,y,z) * 0.5 + 0.5
	end

	#double noise0_1(double x, double y) const
	#{
	#	return noise(x, y) * 0.5 + 0.5;
	#}

	#double noise0_1(double x, double y, double z) const
	#{
	#	return noise(x, y, z) * 0.5 + 0.5;
	#}

	#double octaveNoise0_1(double x, std::int32_t octaves) const
	#{
	#	return octaveNoise(x, octaves) * 0.5 + 0.5;
	#}

	#double octaveNoise0_1(double x, double y, std::int32_t octaves) const
	#{
	#	return octaveNoise(x, y, octaves) * 0.5 + 0.5;
	#}

	#double octaveNoise0_1(double x, double y, double z, std::int32_t octaves) const
	#{
	#	return octaveNoise(x, y, z, octaves) * 0.5 + 0.5;
	#}
end

