#define IX(i,j,k) ((i)+(N+2)*(j) + (N+2)*(N+2)*(k))
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

void add_source(int N, float *value, float *value_prev, float dt)
{
    int count;
    int size=(N+2)*(N+2)*(N+2);
    
    for(count=0; count<size; count++)
    {
        value[count]=value[count]+dt*value_prev[count];
    }
}

void boundary_condition(int N, float *value, int flag)
{
    int count_a;
    int count_b;
    
    for(count_a=1; count_a<=N; count_a++)
    {
        for(count_b=1; count_b<=N; count_b++)
        {
            if(flag == 1)
            {
                value[IX(0 ,count_a, count_b)]=-value[IX(1, count_a, count_b)];
                value[IX(N+1,count_a, count_b)]=-value[IX(N, count_a, count_b)];
            }
            else
            {
                value[IX(0 ,count_a, count_b)]=value[IX(1, count_a, count_b)];
                value[IX(N+1,count_a, count_b)]=value[IX(N, count_a, count_b)];
            }
            
            if(flag == 2)
            {
                value[IX(count_a, 0, count_b)]=-value[IX(count_a, 1, count_b)];
                value[IX(count_a, N+1, count_b)]=-value[IX(count_a, N, count_b)];
            }
            else
            {
                value[IX(count_a, 0, count_b)]=value[IX(count_a, 1, count_b)];
                value[IX(count_a, N+1, count_b)]=value[IX(count_a, N, count_b)];
            }
            
            if(flag == 3)
            {
                value[IX(count_a, count_b, 0)]=-value[IX(count_a, count_b, 1)];
                value[IX(count_a, count_b, N+1)]=-value[IX(count_a, count_b, N)];
            }
            else
            {
                value[IX(count_a, count_b, 0)]=value[IX(count_a, count_b, 1)];
                value[IX(count_a, count_b, N+1)]=value[IX(count_a, count_b, N)];
            }
        }
    }
    
    value[IX(0, 0, 0)]=(float)(1.0/3.0*(value[IX(1, 0, 0)]+value[IX(0, 1, 0)]+value[IX(0, 0, 1)]));
    value[IX(0, N+1, 0)]=(float)(1.0/3.0*(value[IX(1, N+1, 0)]+value[IX(0, N, 0)]+value[IX(0, N+1, 1)]));
    
    value[IX(N+1, 0, 0)]=(float)(1.0/3.0*(value[IX(N, 0, 0)]+value[IX(N+1, 1, 0)]+value[IX(N+1, 0, 1)]));
    value[IX(N+1, N+1, 0)]=(float)(1.0/3.0*(value[IX(N, N+1, 0)]+value[IX(N+1, N, 0)]+value[IX(N+1, N+1, 1)]));
    
    value[IX(0, 0, N+1)]=(float)(1.0/3.0*(value[IX(1, 0, N+1)]+value[IX(0, 1, N+1)]+value[IX(0, 0, N)]));
    value[IX(0, N+1, N+1)]=(float)(1.0/3.0*(value[IX(1, N+1, N+1)]+value[IX(0, N, N+1)]+value[IX(0, N+1, N)]));
    
    value[IX(N+1, 0, N+1)]=(float)(1.0/3.0*(value[IX(N, 0, N+1)]+value[IX(N+1, 1, N+1)]+value[IX(N+1, 0,N)]));
    value[IX(N+1, N+1,N+1)]=(float)(1.0/3.0*(value[IX(N, N+1, N+1)]+value[IX(N+1, N, N+1)]+value[IX(N+1, N+1, N)]));
}

void lin_solve(int N, int b, float *value, float *value_prev, float a, float c)
{
    int count;
    int count_x;
    int count_y;
    int count_z;
    
    for(count=0; count<10; count++)
    {
        for(count_x=1; count_x<=N; count_x++)
        {
            for(count_y=1; count_y<=N; count_y++)
            {
                for(count_z=1; count_z<=N; count_z++)
                {
                    value[IX(count_x, count_y, count_z)]=(value_prev[IX(count_x, count_y, count_z)]+a*(value[IX(count_x-1, count_y, count_z)]+value[IX(count_x+1, count_y, count_z)]+value[IX(count_x, count_y-1, count_z)]+value[IX(count_x, count_y+1, count_z)]+value[IX(count_x, count_y, count_z-1)]+value[IX(count_x, count_y, count_z+1)]))/c;
                }
            }
        }
        
        boundary_condition(N, value, b);
    }
}

void diffuse(int N, int b, float *value, float *value_prev, float diff, float dt)
{
    float alpha=dt*diff*N*N*N;
    
    lin_solve(N, b, value, value_prev, alpha, 1+6*alpha);
}

void advect(int N, int b, float *density, float *density_prev, float *velocity_u, float *velocity_v, float *velocity_w, float dt)
{
    int count_x;
    int count_y;
    int count_z;
    
    int i0;
    int j0;
    int k0;
    
    int i1;
    int j1;
    int k1;
    
    float x;
    float y;
    float z;
    
    float s0;
    float t0;
    float s1;
    float t1;
    float u1;
    float u0;
    
    float dh;
    
    dh=dt*N;
    
    for(count_x=1; count_x<=N; count_x++)
    {
        for(count_y=1; count_y<=N; count_y++)
        {
            for(count_z=1; count_z<=N; count_z++)
            {
                x=count_x-dh*velocity_u[IX(count_x, count_y, count_z)];
                y=count_y-dh*velocity_v[IX(count_x, count_y, count_z)];
                z=count_z-dh*velocity_w[IX(count_x, count_y, count_z)];
                
                if(x < 0.5)
                {
                    x=0.5;
                }
                
                if(x > N+0.5)
                {
                    x=(float)(N+0.5);
                }
                
                if(y < 0.5)
                {
                    y=0.5;
                }
                
                if(y > N+0.5)
                {
                    y=(float)(N+0.5);
                }
                
                if(z<0.5)
                {
                    z=0.5;
                }
                
                if(z>N+0.5)
                {
                    z=(float)(N+0.5);
                }
                
                i0=(int)x;
                i1=i0+1;
                j0=(int)y;
                j1=j0+1;
                k0=(int)z;
                k1=k0+1;
                
                s1=x-i0;
                s0=1-s1;
                t1=y-j0;
                t0=1-t1;
                u1=z-k0;
                u0=1-u1;
                
                density[IX(count_x, count_y, count_z)]=s0*(t0*u0*density_prev[IX(i0,j0,k0)]+t1*u0*density_prev[IX(i0,j1,k0)]+t0*u1*density_prev[IX(i0,j0,k1)]+t1*u1*density_prev[IX(i0,j1,k1)])+
                s1*(t0*u0*density_prev[IX(i1,j0,k0)]+t1*u0*density_prev[IX(i1,j1,k0)]+t0*u1*density_prev[IX(i1,j0,k1)]+t1*u1*density_prev[IX(i1,j1,k1)]);
            }
        }
    }
    
    boundary_condition(N, density, b);
}

void project(int N, float *velocity_u, float *velocity_v, float *velocity_w, float *p, float *div)
{
    int count_x;
    int count_y;
    int count_z;
    
    for(count_x=1; count_x<=N; count_x++)
    {
        for(count_y=1; count_y<=N; count_y++)
        {
            for(count_z=1; count_z<=N; count_z++)
            {
                div[IX(count_x, count_y, count_z)]=(float)(-1.0/3.0*((velocity_u[IX(count_x+1, count_y, count_z)]-velocity_u[IX(count_x-1, count_y, count_z)])/N+(velocity_v[IX(count_x, count_y+1, count_z)]-velocity_v[IX(count_x, count_y-1, count_z)])/N+(velocity_w[IX(count_x, count_y, count_z+1)]-velocity_w[IX(count_x, count_y, count_z-1)])/N));
                p[IX(count_x, count_y, count_z)] = 0;
            }
        }
    }
    
    boundary_condition(N, div, 0);
    boundary_condition(N, p, 0);
    
    lin_solve(N, 0, p, div, 1, 6);
    
    for(count_x=1; count_x<=N; count_x++)
    {
        for(count_y=1; count_y<=N; count_y++)
        {
            for(count_z=1; count_z<=N; count_z++)
            {
                velocity_u[IX(count_x, count_y, count_z)] -= 0.5f*N*(p[IX(count_x+1, count_y, count_z)]-p[IX(count_x-1, count_y, count_z)]);
                velocity_v[IX(count_x, count_y, count_z)] -= 0.5f*N*(p[IX(count_x, count_y+1, count_z)]-p[IX(count_x, count_y-1, count_z)]);
                velocity_w[IX(count_x, count_y, count_z)] -= 0.5f*N*(p[IX(count_x, count_y, count_z+1)]-p[IX(count_x, count_y, count_z-1)]);
            }
        }
    }
    
    boundary_condition(N, velocity_u, 1);
    boundary_condition(N, velocity_v, 2);
    boundary_condition(N, velocity_w, 3);
}

void get_density(int N, float *density, float *density_prev, float *velocity_u, float *velocity_v, float *velocity_w, float diff, float dt)
{
    add_source(N, density, density_prev, dt);
    
    SWAP(density_prev, density);
    diffuse(N, 0, density, density_prev, diff, dt);
    
    SWAP(density_prev, density);
    advect(N, 0, density, density_prev, velocity_u, velocity_v, velocity_w, dt);
}

void get_velocity(int N, float *velocity_u, float *velocity_v,  float *velocity_w, float *velocity_u_prev, float *velocity_v_prev, float *velocity_w_prev, float visc, float dt)
{
    add_source(N, velocity_u, velocity_u_prev, dt);
    add_source(N, velocity_v, velocity_v_prev, dt);
    add_source(N, velocity_w, velocity_w_prev, dt);
    
    SWAP(velocity_u_prev, velocity_u);
    SWAP(velocity_v_prev, velocity_v);
    SWAP(velocity_w_prev, velocity_w);
    
    diffuse(N, 1, velocity_u, velocity_u_prev, visc, dt);
    diffuse(N, 2, velocity_v, velocity_v_prev, visc, dt);
    diffuse(N, 3, velocity_w, velocity_w_prev, visc, dt);
    
    project(N, velocity_u, velocity_v, velocity_w, velocity_u_prev, velocity_v_prev);
    
    SWAP(velocity_u_prev, velocity_u);
    SWAP(velocity_v_prev, velocity_v);
    SWAP(velocity_w_prev, velocity_w);
    
    advect(N, 1, velocity_u, velocity_u_prev, velocity_u_prev, velocity_v_prev, velocity_w_prev, dt);
    advect(N, 2, velocity_v, velocity_v_prev, velocity_u_prev, velocity_v_prev, velocity_w_prev, dt);
    advect(N, 3, velocity_w, velocity_w_prev, velocity_u_prev, velocity_v_prev, velocity_w_prev, dt);
    
    project(N, velocity_u, velocity_v, velocity_w, velocity_u_prev, velocity_v_prev);
}