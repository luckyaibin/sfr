#include <sfr/Common.hpp>
#include <sfr/sfr.hpp>
#include <SFML/Window.hpp>

using namespace sfr;

#define ROWS 1
#define COLS 1

std::auto_ptr<sf::Window> window;
std::auto_ptr<sf::Clock> timer;
Ptr<sfr::AssetTable> assets;
Ptr<sfr::AssetLoader> assetLoader;
Ptr<sfr::DeferredRenderer> deferredRenderer;
Ptr<sfr::TransformUpdater> updater;
Ptr<sfr::BoundsRenderer> boundsRenderer;
Ptr<sfr::Scene> scene;
Ptr<sfr::Transform> camera;
Ptr<sfr::Transform> lightNode;
Ptr<sfr::Particles> particles;
Ptr<sfr::Ribbon> ribbon;
Ptr<sfr::Transform> root;
sf::Time elapsedTime = sf::seconds(0.f);
float z = 0;//3.1f;
float x = -5.f;

namespace sfr {
    extern char const* const resources[];
}

void initWindow() {
    // Initialize the window
    sf::ContextSettings settings(0, 0, 0, 3, 2); // depth/stencil unnecessary w/ deferred
    sf::VideoMode mode(1200, 800, 32);
    window.reset(new sf::Window(mode, "Window", sf::Style::Default, settings));
    //window->setVerticalSyncEnabled(true);
    timer.reset(new sf::Clock);

    settings = window->getSettings();
    if (settings.majorVersion < 3 || (settings.majorVersion == 3 && settings.minorVersion < 2)) {
        throw std::runtime_error("This program requires OpenGL 3.2");
    }

    // Load OpenGL extensions and check for required features
#ifdef SFR_USE_GLEW
    glewExperimental = GL_TRUE;
    GLint error = glewInit();
    if (GLEW_OK != error) {
        throw std::runtime_error((char*)glewGetErrorString(error));
    }
    if (!GLEW_VERSION_3_2) {
        throw std::runtime_error("This program requires OpenGL 3.2");
    }
    std::cerr << glGetString(GL_VERSION) << std::endl;
#endif
    glViewport(0, 0, window->getSize().x, window->getSize().y);

    // Set up the renderer, resources, assets, etc.
    assets.reset(new sfr::AssetTable());
    assetLoader.reset(new sfr::AssetLoader(assets));

    deferredRenderer.reset(new sfr::DeferredRenderer(assets));
    updater.reset(new sfr::TransformUpdater);
    boundsRenderer.reset(new sfr::BoundsRenderer(assets));
    scene.reset(new sfr::Scene());
    root = scene->root();
}


void initCamera() {
    camera = root->childIs<sfr::Transform>("camera");
    Ptr<sfr::Camera> cam = camera->childIs<sfr::Camera>();
    cam->viewportWidthIs(window->getSize().x);
    cam->viewportHeightIs(window->getSize().y);
    scene->cameraIs(cam);
    scene->skyboxIs(assets->assetIs<Cubemap>("textures/Nebula.png"));
    //
    /*
    cam->nearIs(0);
    cam->farIs(100);
    cam->leftIs(-10);
    cam->rightIs(10);
    cam->topIs(10);
    cam->bottomIs(-10);
    cam->typeIs(sfr::Camera::ORTHOGRAPHIC);
*/
}

void initLights() {
    Ptr<sfr::DepthRenderTarget> target1(new sfr::DepthRenderTarget(2048, 2048));
    Ptr<sfr::HemiLight> light1 = root->childIs<sfr::HemiLight>();
    light1->diffuseColorIs(sfr::Color(1., 1., 1., 1.));
    light1->backDiffuseColorIs(sfr::Color(.1f, .1f, .1f, 1.f));
    light1->directionIs(sfr::Vector(1.f, -1.f, 0.f));
    light1->constantAttenuationIs(1);
    light1->linearAttenuationIs(0);
    light1->quadraticAttenuationIs(0);
    light1->shadowMapIs(target1);

    for (int i = -ROWS/2; i < ROWS-ROWS/2; i++) {
        for (int j = -COLS/2; j < COLS-COLS/2; j++) {
            Ptr<sfr::DepthRenderTarget> target(new sfr::DepthRenderTarget(2048, 2048));

            Ptr<sfr::Transform> node = root->childIs<sfr::Transform>("light");
            node->positionIs(sfr::Vector(i * 2.f, 16.f, j * 5.f + 1.f));

            Ptr<sfr::SpotLight> light = node->childIs<sfr::SpotLight>();
            light->spotCutoffIs(20.f);
            light->spotPowerIs(40.f);
            light->constantAttenuationIs(1.f);
            light->linearAttenuationIs(0.f);
            light->quadraticAttenuationIs(0.f);
            light->specularColorIs(sfr::Color(1.f, 1.f, 1.f, 1.f));
            light->diffuseColorIs(sfr::Color(3.f, 3.f, 3.f, 3.f));
            light->directionIs(sfr::Vector(0, -1, 0));
            light->shadowMapIs(target);

            lightNode = node;
        }
    }
}

void handleInput() {
    // Loop until the event queue is empty, and then poll for keys
    sf::Event evt;
    while (window->pollEvent(evt)) {
        switch (evt.type) {
        case sf::Event::Closed:
            std::cout << "Exiting" << std::endl;
			window.reset();
            exit(0);
        case sf::Event::KeyPressed:
            break;
        default:
            break;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        x += 2.f * elapsedTime.asSeconds();
    } 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        x -= 2.f * elapsedTime.asSeconds();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        z -= 2.f * elapsedTime.asSeconds();
    } 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        z += 2.f * elapsedTime.asSeconds();
    }
    if (lightNode) {
        sfr::Vector pos = lightNode->position();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            pos.x += 2.f * elapsedTime.asSeconds();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            pos.x -= 2.f * elapsedTime.asSeconds();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            pos.z += 2.f * elapsedTime.asSeconds();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            pos.z -= 2.f * elapsedTime.asSeconds();
        }
        lightNode->positionIs(pos);
    }

    float const y = 2.f;
    camera->transformIs(sfr::Matrix::look(
        sfr::Vector(x, y, z),
        sfr::Vector(0.f, 0.3f, 0.f),
        sfr::Vector(0.f, 1.f, 0.f)));
/*
    float const y = 8.f;
    camera->transformIs(sfr::Matrix::look(
        sfr::Vector(x, y, z),
        sfr::Vector(0.f, 0.0f, 0.f),
        sfr::Vector(1.f, 0.f, 0.f)));
*/
}

void initTransparency() {
    Ptr<sfr::Transform> shield(assets->assetIs<sfr::Transform>("meshes/Shield.obj"));
    root->childIs(shield);
}

void initModels() {
    // Initialize the models that are part of the scene
    //Ptr<sfr::Transform> plane(assets->assetIs<sfr::Transform>("meshes/Plane.obj"));
    //root->childIs(plane);

    //Ptr<sfr::Transform> sphere = assets->nodeIs("meshes/SmoothSphere.obj");
    //sphere->positionIs(sfr::Vector(0.f, 0.f, 5.f));
    Ptr<sfr::Transform> car(assets->assetIs<sfr::Transform>("meshes/Lexus.obj"));
    //Ptr<sfr::Transform> car(assets->assetIs<sfr::Transform>("meshes/Insurrector.obj"));
    for (int i = -ROWS/2; i < ROWS-ROWS/2; i++) {
        for (int j = -COLS/2; j < COLS-COLS/2; j++) {
            Ptr<sfr::Transform> node = root->childIs<sfr::Transform>("car");
            node->positionIs(sfr::Vector(i * 2.f+1.f, 0.f, j * 5.f));
            node->childIs(car);
        }
    }
}

void initDecals() {
    Ptr<sfr::Decals> decals = root->childIs<sfr::Decals>();
    Ptr<sfr::Texture> tex = assets->assetIs<sfr::Texture>("textures/Checkerboard.png");

    Decal decal;
    decal.normal = sfr::GLvec3(0.f, 1.f, 0.f);
    decal.right = sfr::GLvec3(1.f, 0.f, 0.f);
    decal.position = sfr::GLvec3(0.f, 0.f, 0);
    decal.alpha = 1.f;
    decal.width = 2.f;
    decal.height = 2.f;
    decal.depth = 2.f;

    decals->decalEnq(decal);
    decals->textureIs(tex);

}


void initFonts() {
    Ptr<sfr::Font> font = assets->assetIs<sfr::Font>("fonts/Ethnocentric.ttf");
    Ptr<sfr::Text> text = root->childIs<sfr::Text>();
    text->textIs("zero combat");
    text->fontIs(font);
    text->colorIs(sfr::Color(1.f, .4f, .1f, 1.f));

    Ptr<sfr::Font> font2 = assets->assetIs<sfr::Font>("fonts/Ethnocentric.ttf#40");
    Ptr<sfr::Ui> ui = scene->ui()->childIs<sfr::Ui>();
    ui->xIs(sfr::Coord::center());
    ui->yIs(sfr::Coord::center());
    ui->heightIs(sfr::Span(50, sfr::Span::PIXELS));
    Ptr<sfr::Text> text2 = ui->childIs<sfr::Text>();
    text2->textIs("multiplayer");
    text2->fontIs(font2);
    text2->colorIs(sfr::Color(1.f, 1.f, 1.f, .8f));
    text2->sizeIs(40);
}

void initUiQuad() {
    Ptr<sfr::Ui> ui = scene->ui()->childIs<sfr::Ui>();
    ui->xIs(sfr::Coord(0, sfr::Coord::PIXELS));//::center());
    ui->yIs(sfr::Coord(0, sfr::Coord::PIXELS));//::center());
    //ui->yIs(sfr::Coord::center());
    Ptr<sfr::Quad> quad = ui->childIs<sfr::Quad>();
    quad->textureIs(assets->assetIs<sfr::Texture>("textures/White.png"));
    quad->widthIs(100);
    quad->heightIs(100);
    quad->tintIs(sfr::Color(1.f, 1.f, 1.f, 1.f));
    
}

void initParticles() {
    Ptr<sfr::Transform> pnode = root->childIs<sfr::Transform>("particles");
    particles = pnode->childIs<sfr::Particles>();

    Particle p1 = { GLvec3(0, 0, 0), GLvec3(), GLvec4(1, 1, 1, 1), 1, 0 };
    Particle p2 = { GLvec3(2, 0, 0), GLvec3(), GLvec4(1, 1, 1, 1), 1, 0 };
    particles->particleEnq(p1);
    particles->particleEnq(p2);
    particles->textureIs(assets->assetIs<Texture>("textures/BurstGold.png"));
}

void initRibbon() {
    Ptr<sfr::Transform> rnode = root->childIs<sfr::Transform>("ribbon");
    ribbon = rnode->childIs<sfr::Ribbon>();
    rnode->positionIs(Vector(0, 0, 0));

    ribbon->textureIs(assets->assetIs<Texture>("textures/IncandescentBlue.png"));
    ribbon->widthIs(1);
    ribbon->minWidthIs(0);
    ribbon->pointQuotaIs(10);

    for (float i = -100; i < 2; i += .1f) {
        //ribbon->pointEnq(Vector(i*i, i, 0));
        //ribbon->pointEnq(Vector(0, i, 0));
        ribbon->pointEnq(Vector(0, i, 0));
    }
}

void initQuad() {
    Ptr<sfr::Billboards> billboards = root->childIs<sfr::Billboards>();
    billboards->textureIs(assets->assetIs<Texture>("textures/IncandescentBlue.png"));

    Billboard billboard;
    billboard.width = 2.;
    billboard.height = 2;
    billboard.right = GLvec3(1, 0, 0);
    billboard.forward = GLvec3(0, 1, 0);
    billboard.position = GLvec3(0, 0, 0);
    billboard.color = GLvec4(1, 1, 1, 1);

    billboards->billboardEnq(billboard);

    billboard.position = GLvec3(10, 0, 0);
    billboards->billboardEnq(billboard);
}

void runRenderLoop() {
    sf::Clock perfClock;
    float realTime = 0.f;
    float perfTime = 0.f;
    float perfFrames = 0.f;
    float cpuTime = 0.f;
    GLuint64 gpuTime = 0;

    GLuint queryId = 0;
    glGenQueries(1, &queryId);

    // Run the game loop while the window is still open
    while (window->isOpen()) {
        elapsedTime = timer->getElapsedTime();
        realTime += elapsedTime.asSeconds();
        timer->restart();

        handleInput();

        // Record the CPU time used while traversing the scene graph.  Don't
        // include time processing input or running the Display() function,
        // because that causes the CPU to wait for the GPU to finish rendering.
        perfClock.restart();

        glBeginQuery(GL_TIME_ELAPSED, queryId);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        updater->operator()(scene);
        deferredRenderer->operator()(scene);
        //boundsRenderer->operator()(scene);

        cpuTime += perfClock.getElapsedTime().asSeconds();
        // CPU time used for rendering

        window->display();

        GLuint64 elapsed = 0;
        glEndQuery(GL_TIME_ELAPSED);
        glGetQueryObjectui64v(queryId, GL_QUERY_RESULT, &elapsed);
        gpuTime += elapsed;
        // Total GPU time
        
        perfTime += perfClock.getElapsedTime().asSeconds();
        perfFrames++;
        // Total time 

        // Display the time every couple of seconds.  This is not the total
        // time taken per frame, but the total time used per frame to
        // traverse the scene graph and send the info to the graphics card.
        if (realTime >= 10.f) {
            printf("%06.3f cpu ms/frame\n", cpuTime/perfFrames * 1000.f);
            printf("%06.3f gpu ms/frame\n", gpuTime/1000000.f/perfFrames);
            printf("%06.3f ms/frame (total)\n\n", perfTime/perfFrames * 1000.f);
            perfTime = 0;
            perfFrames = 0;
            realTime = 0;
            cpuTime = 0;
            gpuTime = 0;
        }
    }

    glDeleteQueries(1, &queryId);
}

int main() {

    try {    
        initWindow();
        initCamera();
        initDecals();
        initTransparency();
        initModels();
        initFonts();
        initParticles();
        initRibbon();
        initQuad();
        initLights();
        initUiQuad();
        runRenderLoop();
    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    return 0;
}
