#include <shape.h>

shape::shape()
{
    points = vector<vector<glm::vec2>>();
}

void shape::setup()
{

}

void shape::update(float dt)
{

}

bool shape::load(string filename)
{
    OGRRegisterAll();


    // First open the shape file
    ds = OGRSFDriverRegistrar::Open( filename.c_str(), FALSE );

    // Now to load in the points .....
    points = vector<vector<glm::vec2>>();

    // Grab the first layer
    OGRLayer* layer = ds->GetLayer(0);

    // Grab the spatial reference and create a coordinate transform function
    sr = layer->GetSpatialRef();

    // Taking from http://www.compsci.wm.edu/SciClone/documentation/software/geo/gdal-1.9.0/html/ogr/ogr_apitut.html
    OGRFeature *poFeature;

    layer->ResetReading();
    while ( (poFeature = layer->GetNextFeature()) != NULL )
    {
        OGRFeatureDefn *poFDefn = layer->GetLayerDefn();
        int iField;

        OGRGeometry *poGeometry;

        poGeometry = poFeature->GetGeometryRef();
        if ( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
        {
            OGRPoint *poPoint = (OGRPoint *) poGeometry;
            cout << "POINT!!!" << endl;
            double x = poPoint->getX();
            double y = poPoint->getY();
            points.push_back(vector<glm::vec2>());
            points[points.size() - 1].push_back(glm::vec2(x, y));
        }
        else if (poGeometry != NULL
                 && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
        {
            OGRLineString* ls = (OGRLineString*)poGeometry;
            points.push_back(vector<glm::vec2>());
            for (int i = 0; i < ls->getNumPoints(); i++ )
            {
                OGRPoint p;
                ls->getPoint(i, &p);

                // This function can transform a larget set of points.....
                double x = p.getX();
                double y = p.getY();
                points[points.size() - 1].push_back(glm::vec2(x, y));
                //poTransform->Transform (1, &x, &y);
                //cout << "POINT" << p.getX() << " " << p.getY() << endl;
            }
        }
        else if (poGeometry != NULL
                 && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
        {

            OGRLineString* ls = (OGRLineString*)poGeometry->getBoundary();
            points.push_back(vector<glm::vec2>());
            cout << "POLYGON" << ls->getNumPoints() << endl;
            //exit(0);
            for (int i = 0; i < ls->getNumPoints(); i++ )
            {
                OGRPoint p;
                ls->getPoint(i, &p);

                // This function can transform a larget set of points.....
                double x = p.getX();
                double y = p.getY();
                points[points.size() - 1].push_back(glm::vec2(x, y));
                //poTransform->Transform (1, &x, &y);
            }
        }
        OGRFeature::DestroyFeature( poFeature );
    }
    return true;
}

bool shape::createMesh(OGRSpatialReference* sr2, glm::vec2 origin, glm::vec2 scale, terrain& t)
{
    // Now to pair the points
    vector<Vertex> vertexs = {};
    vector<int> indicies = {};
    Renderer.init();

    cout << "LINE RENDERER CREATOR" << endl;
    if (!Renderer.addShader(GL_VERTEX_SHADER, AssetManager::GetAppPath() + "../../code/shader/linevert.shader"))
    {
        cout << "Failed to add shader" << endl;
        return false;
    }
    if (!Renderer.addShader(GL_FRAGMENT_SHADER, AssetManager::GetAppPath() + "../../code/shader/linefrag.shader"))
    {
        cout << "Failed to add shader" << endl;
        return false;
    }

    if (!Renderer.compile())
    {
        cout << "Failed to compile shaders" << endl;
        return false;
    }
    if (!Renderer.link())
    {
        cout << "Failed to link shaders" << endl;
        return false;
    }
    OGRCoordinateTransformation* transform;
    // Create a coordinate transform
    if (sr != NULL)
    {
        transform = OGRCreateCoordinateTransformation( sr, sr2 );
    }
    else
    {
        return false;
    }
    bool first = true;
    for (int i = 0; i < points.size(); i++)
    {
        //break;
        first = true;
        for (int j = 0; j < points[i].size(); j++)
        {

            double x = points[i][j].x;
            double y = points[i][j].y;
            //points[points.size()-1].push_back(glm::vec2(x,y));
            transform->Transform (1, &x, &y);
            glm::vec2 orig = glm::vec2(x, y);
            x = x - origin.x;
            y = origin.y - y;

            Vertex temp = {{(float)x, (float)t.SampleTerrain(orig), (float)y}, {(float)1, (float)0, (float)0}, {(float)1, (float)1}};
            //cout << "Vertex: " << temp.position.x << " " << temp.position.y << " " << temp.position.z << endl;
            if (first)
            {
                vertexs.push_back(temp);
                temp.position.y += 40;
                vertexs.push_back(temp);
                first = false;
                if (j == points[i].size() - 1)
                {
                    //vertexs.push_back(temp);
                    temp.position.y -= 5;
                    temp.position.z += 5;
                    temp.position.y = t.SampleTerrain2(glm::vec2(temp.position.x,temp.position.z));
                    vertexs.push_back(temp);
                    temp.position.y += 40;

                    vertexs.push_back(temp);
                    indicies.push_back(vertexs.size() - 3);
                    indicies.push_back(vertexs.size() - 1);
                    indicies.push_back(vertexs.size() - 4);
                    indicies.push_back(vertexs.size() - 2);
                    indicies.push_back(vertexs.size() - 4);
                    indicies.push_back(vertexs.size() - 1);
                    continue;
                }
            }
            if (j + 1 < points[i].size())
            {
                x = points[i][j + 1].x;
                y = points[i][j + 1].y;
                //cout << "Before" << x << " " << y << endl;
                transform->Transform (1, &x, &y);
                //cout << "After" << x << " " << y << " " << origin.x << " " << origin.y<< endl;
                orig = glm::vec2(x, y);
                x = x - origin.x;
                y = origin.y - y;

                Vertex temp2 = {{(float)x, (float)t.SampleTerrain(orig), (float)y}, {(float)1, (float)0, (float)0}, {1, 1}};
                //cout << "Vertex: " << temp2.position.x << " " << temp2.position.y << " " << temp2.position.z << endl;
                //cout << "ORIGIN: " << origin.x << " " << origin.y << " " << temp2.position.z << endl;
                vertexs.push_back(temp2);
                temp2.position.y += 40;
                vertexs.push_back(temp2);
                // Triangulate!
                indicies.push_back(vertexs.size() - 3);
                indicies.push_back(vertexs.size() - 1);
                indicies.push_back(vertexs.size() - 4);
                indicies.push_back(vertexs.size() - 2);
                indicies.push_back(vertexs.size() - 4);
                indicies.push_back(vertexs.size() - 1);
            }


        }
    }

    // Generate vbo
    veccount = indicies.size();
    cout << "Allocate VBO" << endl;
    vbo_geometry.generateBuffer(GL_ARRAY_BUFFER);
    vbo_geometry.bindBuffer();
    vbo_geometry.allocateBufferData(sizeof(Vertex)*vertexs.size(), &vertexs[0], GL_STATIC_DRAW);

    // Generate element array
    cout << "Allocate Elements" << endl;
    elements.generateBuffer(GL_ELEMENT_ARRAY_BUFFER);
    elements.bindBuffer();
    elements.allocateBufferData(sizeof(int) * indicies.size(), &indicies[0], GL_STATIC_DRAW);

    return true;
}
void shape::render(glm::mat4& view, glm::mat4& projection)
{
    glm::mat4 mvp;

    mvp = projection * view * model;

    Renderer.useProgram();

    Renderer.setUniformMatrix4x4("mvpMatrix", mvp);
    Renderer.setUniformMatrix4x4("model", model);
    Renderer.enableVertexAttribPointer("v_position");
    //Renderer.enableVertexAttribPointer("v_color");

    vbo_geometry.bindBuffer();

    Renderer.setGLVertexAttribPointer("v_position", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    //Renderer.setGLVertexAttribPointer("v_color",3, GL_FLOAT, GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,color));

    // Index buffer
    elements.bindBuffer();

    Renderer.render(veccount);
    //Renderer.disableVertexAttribPointer("v_color");
    Renderer.disableVertexAttribPointer("v_position");
}
