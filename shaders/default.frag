//δυστηχός παρόλο που η glsl έχει double δεν δουλεύουν οι μαθηματικές πράξεις (sin,log κτλ.)
#version 460 core
out vec4 FragColor;
uniform vec2 resolution;
uniform float zoom;
uniform vec2 pos;
uniform int upscale;
uniform float gamma;
uniform vec2 power;
uniform bool julius_status;
uniform vec2 inpt_c;
uniform int color_inside;

//συνάρτηση για δυνάμοις μιγαδικών αριθμών
vec2 cpow(vec2 z, vec2 p)
{
    vec2 result = vec2(0.0);
    if(z.x != 0)//για κάποιο λόγο, δεν μπορούσα να κάνω τη συνάρτηση να δουλεύει για z.x = 0
    {
        //μετατρέπω την βάση σε πολικό σύστημα συντεταγμένων
        float r_l = log(length(z));
        float theta = atan(z.y,z.x);

        // (r*e^(iθ))^(a+bi) = (r^a*e^(-b*θ)) * (r^b*e^(a*θ))^i = λ * e^(iκ)
        float lamda = exp(r_l*p.x-theta*p.y);
        float kapa = r_l*p.y+theta*p.x;

        // λ*e^iκ = λ*(cos(κ) + sin(κ)i)
        float r_part = lamda*cos(kapa);
        float i_part = lamda*sin(kapa);

        result = vec2(r_part,i_part); //δεν ξέρω αν είναι ο καλήτερος τρόπος αλλά δουλεύει αρκετά καλα
    }
    return result;
}

//για περισσότερες πληροφορίες:
//https://el.wikipedia.org/wiki/Σύνολο_Μάντελμπροτ , https://en.wikipedia.org/wiki/Multibrot_set
float computeInterations_functionset(vec2 uv, float max_iter)
{
    vec2 c = (uv + pos);
    vec2 z = vec2(0.0);
    float iter = 0;
    for(float i; i < max_iter; ++i)
    {
        z = cpow(z,power) + c; //αρχήζω με 0^2+(x + yi) και κητάω αν περνάει το 2 μετα απο πόσες φορές πραγματοποίησα την συνάρτηση
        if(dot(z,z) > 4.0)
        {
            return iter/max_iter; // το εξωτερικό του συνόλου
        }
        ++iter;
    }
    return color_inside;// το εσωτερικό του συνόλου
}

//για περισσότερες πληροφορίες:
//https://en.wikipedia.org/wiki/Julia_set
float computeInterations_juliaset(vec2 uv, float max_iter, vec2 c)
{
    vec2 z = (uv + pos);
    float iter = 0;
    for(float i; i < max_iter; ++i)
    {
        z = cpow(z,power) + c; //αρχήζω με (x + yi)^2+c και κητάω αν περνάει το 2 μετα απο πόσες φορές πραγματοποίησα την συνάρτηση
        if(dot(z,z) > 4.0)
        {
            return iter/max_iter;// το εξωτερικό του συνόλου
        }
        ++iter;
    }
    return color_inside;// το εσωτερικό του συνόλου
}

float antialiasingfunc(vec2 uv, float angles, float distance_x, float distance_y, float iterations, vec2 c)
{
    float final_m = 0;
    //Κοιτάω την θέση που θέλω να εφαρμόσω και παίρνω το μέσο όλων των τριγύρω pixels
    if(julius_status)
    {
        final_m = pow(computeInterations_juliaset(uv, iterations, c),gamma);
        for(float i = 0;i < 360; i += 360/angles)
        {
            float factor_x = distance_x * cos(radians(i));
            float factor_y = distance_y * sin(radians(i));
            final_m +=  pow(computeInterations_juliaset(uv + vec2(factor_x, factor_y), iterations, c),gamma);
        }
    }
    else
    {
        final_m = pow(computeInterations_functionset(uv, iterations),gamma);
        for(float i = 0;i < 360; i += 360/angles)
        {
            float factor_x = distance_x * cos(radians(i));
            float factor_y = distance_y * sin(radians(i));
            final_m +=  pow(computeInterations_functionset(uv + vec2(factor_x, factor_y), iterations),gamma);
        }
    }
    return final_m/(angles+1);
}

void main()
{
    //βρίσκω τις συντεταγμένες για κάθε pixel
    float pixelSize_x = 1.0 / resolution.x;
    float pixelSize_y = 1.0 / resolution.y;
	vec2 uv = (gl_FragCoord.xy - 0.5 * resolution.xy) / (resolution.y*zoom);

    //βρήσκω το χρώμα για κάθε pixel
	vec3 col = vec3(0.0);
    col += antialiasingfunc(uv, 9,pixelSize_x/zoom,pixelSize_y/zoom, upscale , inpt_c);

	FragColor = vec4(col, 1.0);
}
