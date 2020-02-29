#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  Int,   width,  m_width,  (1024))
PARAM_INFO(Height, Int,   height, m_height, (1024))

PARAM_INFO(Quality, terraingraph::NoiseQuality, quality, m_quality, (terraingraph::NoiseQuality::Standard))

PARAM_INFO(Frequency,   Float, frequency,    m_frequency,    (1.0f))
PARAM_INFO(Lacunarity,  Float, lacunarity,   m_lacunarity,   (2.0f))
PARAM_INFO(OctaveCount, Int,   octave_count, m_octave_count, (6))
PARAM_INFO(Seed,        Int,   seed,         m_seed,         (0))
