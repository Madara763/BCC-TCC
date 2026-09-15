import sys
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection


def ler_malha(nome_arquivo):
    with open(nome_arquivo, "r") as arquivo:

        # Primeira linha:
        # n = número de vértices
        # f = número de faces
        n, f = map(int, arquivo.readline().split())

        # --------------------------------------------------
        # Vértices
        # --------------------------------------------------

        vertices = []

        for _ in range(n):
            x, y, z = map(int, arquivo.readline().split())
            vertices.append((x, y, z))

        # --------------------------------------------------
        # Faces
        # --------------------------------------------------

        faces = []

        for _ in range(f):
            indices = list(map(int, arquivo.readline().split()))

            # Os índices do arquivo começam em 1.
            # Python utiliza índices começando em 0.
            face = [i - 1 for i in indices]

            faces.append(face)

    return vertices, faces


def desenhar_malha(ax, vertices, faces, nome, cor):

    # --------------------------------------------------
    # Faces
    # --------------------------------------------------

    poligonos = []

    for face in faces:
        pontos = [vertices[i] for i in face]
        poligonos.append(pontos)

    colecao = Poly3DCollection(
        poligonos,
        alpha=0.35,
        facecolor=cor,
        edgecolor="black",
        linewidth=1
    )

    ax.add_collection3d(colecao)

    # --------------------------------------------------
    # Arestas
    # --------------------------------------------------

    for face in faces:

        for i in range(len(face)):

            v1 = vertices[face[i]]
            v2 = vertices[face[(i + 1) % len(face)]]

            ax.plot(
                [v1[0], v2[0]],
                [v1[1], v2[1]],
                [v1[2], v2[2]],
                color="black",
                linewidth=1
            )

    # --------------------------------------------------
    # Vértices
    # --------------------------------------------------

    xs = [v[0] for v in vertices]
    ys = [v[1] for v in vertices]
    zs = [v[2] for v in vertices]

    ax.scatter(
        xs,
        ys,
        zs,
        color=cor,
        edgecolor="black",
        s=40
    )

    # --------------------------------------------------
    # Índices dos vértices
    # --------------------------------------------------

    for i, (x, y, z) in enumerate(vertices):

        ax.text(
            x,
            y,
            z,
            f"  {i + 1}",
            fontsize=9
        )

    # --------------------------------------------------
    # Nome da malha
    # --------------------------------------------------

    # Cria um ponto representativo para a legenda
    ax.plot(
        [],
        [],
        [],
        color=cor,
        linewidth=5,
        label=nome
    )


def configurar_eixos(ax, malhas):

    # Junta todos os vértices das duas malhas
    vertices = []

    for vertices_malha, _ in malhas:
        vertices.extend(vertices_malha)

    xs = [v[0] for v in vertices]
    ys = [v[1] for v in vertices]
    zs = [v[2] for v in vertices]

    min_x = min(xs)
    max_x = max(xs)

    min_y = min(ys)
    max_y = max(ys)

    min_z = min(zs)
    max_z = max(zs)

    # Maior dimensão
    max_range = max(
        max_x - min_x,
        max_y - min_y,
        max_z - min_z
    ) / 2

    # Centro do conjunto
    mid_x = (max_x + min_x) / 2
    mid_y = (max_y + min_y) / 2
    mid_z = (max_z + min_z) / 2

    # Adiciona uma pequena margem
    margem = max_range * 0.1

    max_range += margem

    ax.set_xlim(
        mid_x - max_range,
        mid_x + max_range
    )

    ax.set_ylim(
        mid_y - max_range,
        mid_y + max_range
    )

    ax.set_zlim(
        mid_z - max_range,
        mid_z + max_range
    )


def main():

    # --------------------------------------------------
    # Verifica os argumentos
    # --------------------------------------------------

    if len(sys.argv) != 3:

        print(
            f"Uso: {sys.argv[0]} <malha1> <malha2>"
        )

        sys.exit(1)

    arquivo1 = sys.argv[1]
    arquivo2 = sys.argv[2]

    # --------------------------------------------------
    # Lê as duas malhas
    # --------------------------------------------------

    vertices1, faces1 = ler_malha(arquivo1)
    vertices2, faces2 = ler_malha(arquivo2)

    malha1 = (vertices1, faces1)
    malha2 = (vertices2, faces2)

    # --------------------------------------------------
    # Cria a figura
    # --------------------------------------------------

    fig = plt.figure(figsize=(10, 8))

    ax = fig.add_subplot(
        111,
        projection="3d"
    )

    # --------------------------------------------------
    # Desenha as duas malhas
    # --------------------------------------------------

    desenhar_malha(
        ax,
        vertices1,
        faces1,
        arquivo1,
        "red"
    )

    desenhar_malha(
        ax,
        vertices2,
        faces2,
        arquivo2,
        "blue"
    )

    # --------------------------------------------------
    # Configuração dos eixos
    # --------------------------------------------------

    configurar_eixos(
        ax,
        [malha1, malha2]
    )

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")

    ax.set_title(
        "Visualização de duas malhas 3D"
    )

    ax.legend()

    # Permite visualizar melhor a profundidade
    ax.set_box_aspect((1, 1, 1))

    plt.show()


if __name__ == "__main__":
    main()
